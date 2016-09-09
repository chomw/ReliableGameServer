set DST_DIR=./
set SRC_DIR=./
protoc -I=%SRC_DIR% --cpp_out=%DST_DIR% %SRC_DIR%/ls_packet.proto