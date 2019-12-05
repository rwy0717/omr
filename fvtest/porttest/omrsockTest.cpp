/*******************************************************************************
 * Copyright (c) 2019, 2019 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "omrport.h"
#include "omrportsock.h"
#include "testHelpers.hpp"

/**
 * Start a server which creates a socket, binds to an address/port and
 * listens for clients.
 *
 * @param[in] portLibrary
 * @param[in] addrStr The address of the server.
 * @param[in] port The server port.
 * @param[in] family Socket address family wanted.
 * @param[out] serverSocket A pointer to the server socket.
 * @param[out] serverAddr The socket address of the server created.
 *
 * @return 0 on success, return an error otherwise.
 */
int32_t
start_server(struct OMRPortLibrary *portLibrary, const char *addrStr, const char *port, int32_t family,
        omrsock_socket_t *serverSocket, omrsock_sockaddr_t serverAddr)
{
	return OMRPORT_ERROR_NOTEXIST;
}

/**
 * Create the client socket, and then connect to the server.
 *
 * @param[in] portLibrary
 * @param[in] addrStr The address of the server.
 * @param[in] port The server port.
 * @param[in] family Socket address family wanted.
 * @param[in] sessionClientSocket A pointer to the client socket.
 * @param[in] sessionClientAddr The socket address of the client created.
 *
 * @return 0 on success, return an error otherwise.
 */
int32_t
connect_client_to_server(struct OMRPortLibrary *portLibrary, const char *addrStr, const char *port, int32_t family,
        omrsock_socket_t *sessionClientSocket, omrsock_sockaddr_t sessionClientAddr)
{
	return OMRPORT_ERROR_NOTEXIST;
}

/**
 * Test if port library is properly set up to run OMRSock tests.
 *
 * This will iterate through all OMRSock functions in @ref omrsock.c and simply check
 * that the location of the function is not NULL.
 *
 * @note Errors such as an OMRSock function call with a NULL address will be reported.
 */
TEST(PortSockTest, library_function_pointers_not_null)
{
	/* Unimplemented. */
}

/**
 * Test the omrsock per thread buffer.
 *
 * In this test, per thread buffer related functions set up a buffer to store
 * information such as the hints structures created in @ref omrsock_getaddrinfo_create_hints.
 */
TEST(PortSockTest, per_thread_buffer_functionality)
{
	/* Unimplemented. */
}

/**
 * Test @ref omrsock_getaddrinfo_create_hints, @ref omrsock_getaddrinfo,
 * @ref omrsock_freeaddrinfo and all functions that extract details from
 * @ref omrsock_getaddrinfo results.
 *
 * In this test, the relevant variables are passed into
 * @ref omrsock_getaddrinfo_create_hints. The generated hints are passed into
 * @ref omrsock_getaddrinfo. The results generated are used to create a socket.
 *
 * Address families tested will include IPv4, IPv6(if supported on machine),
 * unspecified family. Socket types tested will include Stream and Datagram.
 *
 * @note Errors such as failed function calls, and/or returning the wrong family or
 * socket type from @ref omrsock_getaddrinfo compared to the ones passed into hints,
 * will be reported.
 */
TEST(PortSockTest, getaddrinfo_creation_and_extraction)
{
	/* Unimplemented. */
}

/**
 * Test functions to set up a connection by using two sockets, which talk to each other.
 *
 * First, the server starts and listens for connections. Then, the
 * client starts and sends a request to connect to the server. The messages are
 * sent both ways, and it is checked if they were sent correctly.
 *
 * Address families tested include IPv4 and IPv6 (if supported). Socket types
 * tested include stream and datagram.
 *
 * @note Errors such as failed function calls, failure to create server and/or client, wrong
 * message sent/received, will be reported.
 */
TEST(PortSockTest, two_socket_communication)
{
	/* Unimplemented. */
}
