.PHONY: build install

all: build install

default: build install

build:
	pebble build

install:
	pebble install --phone 172.26.3.33

debug: all logs
	
logs:
	pebble logs --phone 172.26.3.33
