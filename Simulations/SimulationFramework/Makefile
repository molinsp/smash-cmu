export REPO_ROOT = $(HOME)/dev/smash-cmu

all:
	$(MAKE) -C V-RepMadaraQuadrotorControlPlugin
	$(MAKE) -C V-RepMadaraClientPlugin
	$(MAKE) -C V-RepBridgeAlgorithmPlugin
	$(MAKE) -C MadaraQuadrotorControl
	$(MAKE) -C DroneControllerSimulator

clean:
	$(MAKE) -C V-RepMadaraQuadrotorControlPlugin clean
	$(MAKE) -C V-RepMadaraClientPlugin clean
	$(MAKE) -C V-RepBridgeAlgorithmPlugin clean
	$(MAKE) -C MadaraQuadrotorControl clean
	$(MAKE) -C DroneControllerSimulator clean
