export REPO_ROOT = $(HOME)/dev/smash-cmu

all:
	$(MAKE) plugins
	$(MAKE) tests

plugins:
	$(MAKE) -C V-RepMadaraQuadrotorControlPlugin

tests:
	$(MAKE) -C GoToLocation

clean:
	$(MAKE) -C V-RepMadaraQuadrotorControlPlugin clean
	$(MAKE) -C GoToLocation clean
