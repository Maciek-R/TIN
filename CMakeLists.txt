cmake_minimum_required(VERSION 3.0)
project(TIN)
set(CMAKE_CXX_STANDARD 14)

file(GLOB client_SOURCES "Client/*.h" "Client/*.cpp" "Common/*.cpp" "Common/*.h")
file(GLOB ticketserver_SOURCES "TicketServer/*.h" "TicketServer/*.cpp" "Common/*.cpp" "Common/*.h")
file(GLOB serviceserver_SOURCES "ServiceServer/*.h" "ServiceServer/*.cpp" "Common/*.cpp" "Common/*.h")

add_executable(client ${client_SOURCES})
add_executable(ticketserver ${ticketserver_SOURCES})
add_executable(serviceserver ${serviceserver_SOURCES})

set(CMAKE_CXX_FLAGS -DBUILD_WLAN)

set(OPENSSL_LIBRARIES
    -L$/usr/lib/x86_64-linux-gnu
    -lssl libssl.a
    -lcrypto libcrypto.a)


target_link_libraries(
client
${OPENSSL_LIBRARIES}
)

target_link_libraries(
ticketserver
${OPENSSL_LIBRARIES}
)


file(COPY "Common/database" DESTINATION "./")
#add_custom_command(TARGET client POST_BUILD
#    COMMAND -E COPY "Common/database" "./"
#)
#add_custom_command(TARGET client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "Common/database" $<TARGET_FILE_DIR:client>)

