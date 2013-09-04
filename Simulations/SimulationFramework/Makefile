JOBS=6
export REPO_ROOT = $(HOME)/smash-cmu

all:
	@for d in `ls -d */`; do make -C $$d -j $(JOBS); done

clean:
	@for d in `ls -d */`; do make -C $$d clean; done

realclean:
	@for d in `ls -d */`; do make -C $$d realclean; done
