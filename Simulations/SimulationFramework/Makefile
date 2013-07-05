export REPO_ROOT = $(HOME)/dev/smash-cmu

all:
	@for d in `ls -d */`; do make -C $$d; done

clean:
	@for d in `ls -d */`; do make -C $$d clean; done
