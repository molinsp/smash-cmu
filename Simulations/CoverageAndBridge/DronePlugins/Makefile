all:
	$(MAKE) plugins
	$(MAKE) tests

plugins:
	$(MAKE) -C V-RepBridgeAlgorithmPlugin
	$(MAKE) -C V-RepMadaraClientPlugin

tests:
	$(MAKE) -C GoToLocation

clean:
	$(MAKE) -C V-RepBridgeAlgorithmPlugin clean
	$(MAKE) -C V-RepMadaraClientPlugin clean
	$(MAKE) -C GoToLocation clean
