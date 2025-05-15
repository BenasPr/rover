# Makefile in accordance with the docs on git management (to use in combination with meta)
.PHONY: build start clean test fetch-roverlib-c

BUILD_DIR=bin/
BINARY_NAME=c-example

# If not using VSCode's devcontainers, with docker installed you can run this command to
# build the service inside the container.
build-docker:
	docker build -t ase-service-c -f .devcontainer/Dockerfile .
	docker run -it --cap-add=SYS_PTRACE \
		--security-opt seccomp=unconfined \
		--privileged \
		--user=dev:dev \
		-v "`pwd`":/workspaces/work \
		-w /workspaces/work \
		ase-service-c bash -ic 'make build -C /workspaces/work'

# This target clones roverlib-c so that it can be compiled along side the service
fetch-roverlib-c:
	@if [ ! -d "lib" ] || [ -z "$$(ls -A lib 2>/dev/null)" ]; then \
		echo "roverlib-c directory doesn't exist or is empty. Cloning into lib..."; \
		git clone https://github.com/VU-ASE/roverlib-c.git lib; \
	elif [ ! -d "lib/.git" ]; then \
		echo "directory lib exists but is not a git repository. Recloning..."; \
		rm -rf lib; \
		git clone https://github.com/VU-ASE/roverlib-c.git lib; \
	else \
		echo "getting latest roverlib-c"; \
		cd lib && git pull; \
	fi

build: fetch-roverlib-c
	@echo "building ${BINARY_NAME}"
	@mkdir -p $(BUILD_DIR)
	gcc -o $(BUILD_DIR)$(BINARY_NAME) \
	./src/*.c ./lib/src/*.c ./lib/src/rovercom/outputs/*.c ./lib/src/rovercom/tuning/*.c \
	-lcjson -lzmq -lprotobuf-c -lhashtable -llist -I/usr/include/cjson -I./lib/include -g

start: build
	@echo "starting ${BINARY_NAME}"
	./${BUILD_DIR}${BINARY_NAME} 

clean:
	@echo "Cleaning all targets for ${BINARY_NAME}"
	rm -rf $(BUILD_DIR)

test: 
	@echo "No tests configured"
