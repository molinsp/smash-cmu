/*******************************************************************************
 * DroneRK_Transport_Read_Thread.cpp
 *
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *
 * Custom Madara transport for DroneRK
 ******************************************************************************/

#include "DroneRK_Transport_Read_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"
#include "madara/transport/Reduced_Message_Header.h"
#include "ace/Time_Value.h"
#include "drk.h"

#include <iostream>

DroneRK_Transport_Read_Thread::DroneRK_Transport_Read_Thread(
  const Madara::Transport::Settings & settings, const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const ACE_INET_Addr & address) :
  settings_(settings), id_(id), context_(context), barrier_(2),
  terminated_(false), mutex_(), is_not_ready_(mutex_), is_ready_(false),
  address_(address.get_port_number())
{
  // for receiving, we only want to bind to the local port
  ACE_INET_Addr local(address.get_port_number());

  // TODO: perform any necessary drk long range radio initialization

  if(-1 == socket_.open(local, 2, 0, 1))
  {
    MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
      DLINFO "DroneRK_Transport_Read_Thread::DroneRK_Transport_Read_Thread:" \
      " Error subscribing to DroneRK address %s:%d\n",
      address_.get_host_addr(), address_.get_port_number()));
  } 
  else
  {
    MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
      DLINFO "DroneRK_Transport_Read_Thread::DroneRK_Transport_Read_Thread:" \
      " Success subscribing to DroneRK address %s:%d\n",
      address_.get_host_addr(), address_.get_port_number()));
  }
  
  // check for an on_data_received ruleset
  if(settings_.on_data_received_logic.length() != 0)
  {
    MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
      DLINFO "DroneRK_Transport_Read_Thread::DroneRK_Transport_Read_Thread:" \
      " setting rules to %s\n", 
      settings_.on_data_received_logic.c_str()));

    Madara::Expression_Tree::Interpreter interpreter;
    on_data_received_ = interpreter.interpret(context_,
      settings_.on_data_received_logic);
  }
  else
  {
    MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
      DLINFO "DroneRK_Transport_Read_Thread::DroneRK_Transport_Read_Thread:" \
      " no permanent rules were set\n"));
  }
  
  // setup the receive buffer
  if(settings_.queue_length > 0)
    buffer_ = new char [settings_.queue_length];

  this->activate(THR_NEW_LWP | THR_DETACHED, 1);

  MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
    DLINFO "DroneRK_Transport_Read_Thread::DroneRK_Transport_Read_Thread:" \
    " read thread started\n"));
}

DroneRK_Transport_Read_Thread::~DroneRK_Transport_Read_Thread()
{
  // TODO: any potential drk long range radio cleanup

  // close DroneRK port
  if(-1 == socket_.close())
  { 
    MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
      DLINFO "DroneRK_Transport_Read_Thread::close:" \
      " Error closing DroneRK socket\n"));
  }
}

int DroneRK_Transport_Read_Thread::close(void)
{
  terminated_ = true;

  enter_barrier();

  return 0;
}

void DroneRK_Transport_Read_Thread::wait_for_ready(void)
{
  if(!is_ready_)
    is_not_ready_.wait();
}

int DroneRK_Transport_Read_Thread::enter_barrier(void)
{
  MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
    DLINFO "DroneRK_Transport_Read_Thread::enter_barrier:" \
    " beginning thread barrier for shut down of read thread\n"));

  barrier_.wait();
  return 0;
}


int DroneRK_Transport_Read_Thread::svc(void)
{
    ACE_Time_Value wait_time(1);
    ACE_INET_Addr  remote;
    
    // allocate a buffer to send
    char * buffer = buffer_.get_ptr();
    int64_t buffer_remaining = settings_.queue_length;

    while(false == terminated_.value())
    {
        if(buffer == 0)
        {
            MADARA_DEBUG(MADARA_LOG_EMERGENCY,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " Unable to allocate buffer of size %d. Exiting thread.\n",
                settings_.queue_length));
        
            break;
        }

        // read the message from wifi
        int bytes_read = socket_.recv((void *)buffer, 
          settings_.queue_length, remote, 0, &wait_time);
        int bytes_read = 0;

        MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
          DLINFO "DroneRK_Transport_Read_Thread::svc:" \
          " received a message header of %d bytes from %s:%d\n",
          bytes_read,
          remote.get_host_addr(), remote.get_port_number()));
        
        if(bytes_read > 0)
        {
          buffer_remaining =(int64_t)bytes_read;
          bool is_reduced = false;
          Madara::Transport::Message_Header* header;
          
          // check the buffer for a reduced message header
          if(Madara::Transport::Reduced_Message_Header::reduced_message_header_test(buffer))
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " processing reduced KaRL message from %s:%d\n",
              remote.get_host_addr(), remote.get_port_number()));

            header = new Madara::Transport::Reduced_Message_Header();
            is_reduced = true;
          }
          else if(Madara::Transport::Message_Header::message_header_test(buffer))
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " processing KaRL message from %s:%d\n",
              remote.get_host_addr(), remote.get_port_number()));
            
            header = new Madara::Transport::Message_Header();
          }
          else
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " dropping non-KaRL message from %s:%d\n",
              remote.get_host_addr(), remote.get_port_number()));

            // we do not need to delete the header as it was never allocated
            continue;
          }
           
          char * update = header->read(buffer, buffer_remaining);

          if(!is_reduced)
          {
            // reject the message if it is us as the originator(no update necessary)
            if(strncmp(header->originator, id_.c_str(),
              std::min(sizeof(header->originator), id_.size())) == 0)
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " dropping message from ourself\n",
                remote.get_host_addr(), remote.get_port_number()));

              // delete the header and continue to the svc loop
              delete header;
              continue;
            }
            else
            {
              MADARA_DEBUG(MADARA_LOG_MINOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " remote id(%s:%d) is not our own\n",
                remote.get_host_addr(), remote.get_port_number()));
            }

            // reject the message if it is from a different domain
            if(strncmp(header->domain, settings_.domains.c_str(),
              std::min(sizeof(header->domain), settings_.domains.size())) != 0)
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " remote id(%s:%d) in a different domain(%s). Dropping message.\n",
                remote.get_host_addr(), remote.get_port_number(),
                header->domain));

              // delete the header and continue to the svc loop
              delete header;
              continue;
            }
            else
            {
              MADARA_DEBUG(MADARA_LOG_MINOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " remote id(%s:%d) message is in our domain\n",
                remote.get_host_addr(), remote.get_port_number()));
            }
          }

          MADARA_DEBUG(MADARA_LOG_MINOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " iterating over the %d updates\n",
            header->updates));
          
          MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " locking context\n"));
          
          // temporary record for reading from the updates buffer
          Madara::Knowledge_Record record;
          record.clock = header->clock;
          record.quality = header->quality;
          std::string key;

          // lock the context
          context_.lock();
          
          MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " past the lock\n"));

          // iterate over the updates
          for(uint32_t i = 0; i < header->updates; ++i)
          {
            // read converts everything into host format from the update stream
            update = record.read(update, key, buffer_remaining);
            
            if(buffer_remaining < 0)
            {
              MADARA_DEBUG(MADARA_LOG_EMERGENCY,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " unable to process message. Buffer remaining is negative." \
                " Server is likely being targeted by custom KaRL tools.\n"));

              // cleaning up header is done after the for loop
              break;
            }

            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " attempting to apply %s=%s\n",
              key.c_str(), record.to_string().c_str()));
            
            int result = record.apply(context_, key, header->quality,
              header->clock, false);

            if(result != 1)
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " update %s=%s was rejected\n",
                key.c_str(), record.to_string().c_str()));
            }
            else
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " update %s=%s was accepted\n",
                key.c_str(), record.to_string().c_str()));
            }
          }
          
          // before we send to others, we first execute rules
          if(settings_.on_data_received_logic.length() != 0)
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " evaluating rules in %s\n", 
              settings_.on_data_received_logic.c_str()));

            on_data_received_.evaluate();
          }
          else
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " no permanent rules were set\n"));
          }

          // unlock the context
          context_.unlock();
          context_.set_changed();

          // delete header
          delete header;
        }
        else
        {
          MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " wait timeout on new messages from wifi. Proceeding to next wait\n"));
        }

        // read the message from long range radio
        // TODO: ensure this works
//        printf("buffer:\n");
//        for(unsigned int i = 0; i < 50; ++i)
//        {
//            printf("%04X: 0x", i);
//            for(unsigned int j = 0; j < 4; ++j)
//                printf(" %02X", buffer[i * 4 + j]);
//            printf("    ");
//            for(unsigned int j = 0; j < 4; ++j)
//                printf("%c", buffer[i * 4 + j]);
//            printf("\n");
//        }
//        char temp_buffer[2000];
        bytes_read = drk_modem_rx(buffer);
//        printf("temp_buffer:\n");
//        for(unsigned int i = 0; i < 50; ++i)
//        {
//            printf("%04X: 0x", i);
//            for(unsigned int j = 0; j < 4; ++j)
//                printf(" %02X", temp_buffer[i * 4 + j]);
//            printf("    ");
//            for(unsigned int j = 0; j < 4; ++j)
//                printf("%c", temp_buffer[i * 4 + j]);
//            printf("\n");
//        }

        MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
          DLINFO "DroneRK_Transport_Read_Thread::svc:" \
          " received a message header of %d bytes from %s:%d\n",
          bytes_read,
          remote.get_host_addr(), remote.get_port_number()));
        
        if(bytes_read > 0)
        {
          buffer_remaining =(int64_t)bytes_read;
          bool is_reduced = false;
          Madara::Transport::Message_Header* header;
          
          // check the buffer for a reduced message header
          if(Madara::Transport::Reduced_Message_Header::reduced_message_header_test(buffer))
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " processing reduced KaRL message from %s:%d\n",
              remote.get_host_addr(), remote.get_port_number()));

            header = new Madara::Transport::Reduced_Message_Header();
            is_reduced = true;
          }
          else if(Madara::Transport::Message_Header::message_header_test(buffer))
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " processing KaRL message from %s:%d\n",
              remote.get_host_addr(), remote.get_port_number()));
            
            header = new Madara::Transport::Message_Header();
          }
          else
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " dropping non-KaRL message from %s:%d\n",
              remote.get_host_addr(), remote.get_port_number()));

            // we do not need to delete the header as it was never allocated
            continue;
          }
           
          char * update = header->read(buffer, buffer_remaining);

          if(!is_reduced)
          {
            // reject the message if it is us as the originator(no update necessary)
            if(strncmp(header->originator, id_.c_str(),
              std::min(sizeof(header->originator), id_.size())) == 0)
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " dropping message from ourself\n",
                remote.get_host_addr(), remote.get_port_number()));

              // delete the header and continue to the svc loop
              delete header;
              continue;
            }
            else
            {
              MADARA_DEBUG(MADARA_LOG_MINOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " remote id(%s:%d) is not our own\n",
                remote.get_host_addr(), remote.get_port_number()));
            }

            // reject the message if it is from a different domain
            if(strncmp(header->domain, settings_.domains.c_str(),
              std::min(sizeof(header->domain), settings_.domains.size())) != 0)
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " remote id(%s:%d) in a different domain(%s). Dropping message.\n",
                remote.get_host_addr(), remote.get_port_number(),
                header->domain));

              // delete the header and continue to the svc loop
              delete header;
              continue;
            }
            else
            {
              MADARA_DEBUG(MADARA_LOG_MINOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " remote id(%s:%d) message is in our domain\n",
                remote.get_host_addr(), remote.get_port_number()));
            }
          }

          MADARA_DEBUG(MADARA_LOG_MINOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " iterating over the %d updates\n",
            header->updates));
          
          MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " locking context\n"));
          
          // temporary record for reading from the updates buffer
          Madara::Knowledge_Record record;
          record.clock = header->clock;
          record.quality = header->quality;
          std::string key;

          // lock the context
          context_.lock();
          
          MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " past the lock\n"));

          // iterate over the updates
          for(uint32_t i = 0; i < header->updates; ++i)
          {
            // read converts everything into host format from the update stream
            update = record.read(update, key, buffer_remaining);
            
            if(buffer_remaining < 0)
            {
              MADARA_DEBUG(MADARA_LOG_EMERGENCY,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " unable to process message. Buffer remaining is negative." \
                " Server is likely being targeted by custom KaRL tools.\n"));

              // cleaning up header is done after the for loop
              break;
            }

            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " attempting to apply %s=%s\n",
              key.c_str(), record.to_string().c_str()));
            
            int result = record.apply(context_, key, header->quality,
              header->clock, false);

            if(result != 1)
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " update %s=%s was rejected\n",
                key.c_str(), record.to_string().c_str()));
            }
            else
            {
              MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
                DLINFO "DroneRK_Transport_Read_Thread::svc:" \
                " update %s=%s was accepted\n",
                key.c_str(), record.to_string().c_str()));
            }
          }
          
          // before we send to others, we first execute rules
          if(settings_.on_data_received_logic.length() != 0)
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " evaluating rules in %s\n", 
              settings_.on_data_received_logic.c_str()));

            on_data_received_.evaluate();
          }
          else
          {
            MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
              DLINFO "DroneRK_Transport_Read_Thread::svc:" \
              " no permanent rules were set\n"));
          }

          // unlock the context
          context_.unlock();
          context_.set_changed();

          // delete header
          delete header;
        }
        else
        {
          MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
            DLINFO "DroneRK_Transport_Read_Thread::svc:" \
            " wait timeout on new messages from long range. Proceeding to next wait\n"));
        }



    }
    
    MADARA_DEBUG(MADARA_LOG_MAJOR_EVENT,(LM_DEBUG, 
      DLINFO "DroneRK_Transport_Read_Thread::svc:" \
      " entering barrier.\n"));

    barrier_.wait();
    return 0;
}
