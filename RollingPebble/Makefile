PHONE=172.26.3.33

.PHONY: build install

all: build install

default: build install

build:
	pebble build

install:
	pebble install --phone $(PHONE)

debug: all logs

logs:
	pebble logs --phone $(PHONE)
