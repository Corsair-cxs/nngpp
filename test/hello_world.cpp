#include <catch2/catch_all.hpp>
#include <nngpp/nngpp.h>
#include <nngpp/protocol/req0.h>
#include <nngpp/protocol/rep0.h>
#include <nngpp/transport/tls.h>
#include <nngpp/platform/platform.h>

using namespace nng;

namespace {

static const char cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDLjCCAhYCCQDtskvsw6K+mDANBgkqhkiG9w0BAQsFADBYMQswCQYDVQQGEwJV\n"
    "UzELMAkGA1UECAwCQ0ExEjAQBgNVBAcMCVNhbiBEaWVnbzEUMBIGA1UECgwLbmFu\n"
    "b21zZy5vcmcxEjAQBgNVBAMMCWxvY2FsaG9zdDAgFw0xODAxMTEyMjM0MzVaGA8y\n"
    "MTE3MTIxODIyMzQzNVowWDELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAkNBMRIwEAYD\n"
    "VQQHDAlTYW4gRGllZ28xFDASBgNVBAoMC25hbm9tc2cub3JnMRIwEAYDVQQDDAls\n"
    "b2NhbGhvc3QwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDMvoHdEnfO\n"
    "hmG3PTj6YC5qz6N5hgmcwf4EZkor4+R1Q5hDOKqOknWmVuGBD5mA61ObK76vycIT\n"
    "Tp+H+vKvfgunySZrlyYg8IbgoDbvVgj9RF8xFHdN0PVeqnkBCsCzLtSu6TP8PSgI\n"
    "SKiRMH0NUSakWqCPEc2E1r1CKdOpa7av/Na30LPsuKFcAUhu7QiVYfER86ktrO8G\n"
    "F2PeVy44Q8RkiLw8uhU0bpAflqkR1KCjOLajw1eL3C+Io75Io8qUOLxWc3LH0hl3\n"
    "oEI0jWu7JYlRAw/O7xm4pcGTwy5L8Odz4a7ZTAmuapFRarGOIcDg8Yr0tllRd1mH\n"
    "1T4Z2Wv7Rs0tAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAIfUXK7UonrYAOrlXUHH\n"
    "gfHNdOXMzQP2Ms6Sxov+1tCTfgsYE65Mggo7hRJUqmKpstpbdRBVXhTyht/xjyTz\n"
    "5sMjoeCyv1tXOHpLTfD3LBXwYZwsFdoLS1UHhD3qiYjCyyY2LWa6S786CtlcbCvu\n"
    "Uij2q8zJ4WFrNqAzxZtsTfg16/6JRFw9zpVSCNlHqCxNQxzWucbmUFTiWn9rnc/N\n"
    "r7utG4JsDPZbEI6QS43R7gGLDF7s0ftWKqzlQiZEtuDQh2p7Uejbft8XmZd/VuV/\n"
    "dFMXOO1rleU0lWAJcXWOWHH3er0fivu2ISL8fRjjikYvhRGxtkwC0kPDa2Ntzgd3\n"
    "Hsg=\n"
    "-----END CERTIFICATE-----\n";

static const char key[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpQIBAAKCAQEAzL6B3RJ3zoZhtz04+mAuas+jeYYJnMH+BGZKK+PkdUOYQziq\n"
    "jpJ1plbhgQ+ZgOtTmyu+r8nCE06fh/ryr34Lp8kma5cmIPCG4KA271YI/URfMRR3\n"
    "TdD1Xqp5AQrAsy7Urukz/D0oCEiokTB9DVEmpFqgjxHNhNa9QinTqWu2r/zWt9Cz\n"
    "7LihXAFIbu0IlWHxEfOpLazvBhdj3lcuOEPEZIi8PLoVNG6QH5apEdSgozi2o8NX\n"
    "i9wviKO+SKPKlDi8VnNyx9IZd6BCNI1ruyWJUQMPzu8ZuKXBk8MuS/Dnc+Gu2UwJ\n"
    "rmqRUWqxjiHA4PGK9LZZUXdZh9U+Gdlr+0bNLQIDAQABAoIBAC82HqvjfkzZH98o\n"
    "9uKFGy72AjQbfEvxT6mkDKZiPmPr2khl4K5Ph2F71zPzbOoVWYoGZEoUs/PPxWmN\n"
    "rDhbUES4VWupxtkBnZheWUyHAjukcG7Y0UnYTTwvAwgCerzWp6RNkfcwAvMmDfis\n"
    "vak8dTSg0TUsXb+r5KhFDNGcTNv3f7R0cJmaZ/t9FT7SerXf1LW7itvTjRor8/ZK\n"
    "KPwT4oklp1o6RFXSenn/e2e3rAjI+TEwJA3Zp5dqO/M/AhaZKVaxL4voDVdVVkT+\n"
    "LHJWVhjLY5ilPkmPWqmZ2reTaF+gGSSjAQ+t/ahGWFqEdWIz9UoXhBBOd1ibeyvd\n"
    "Kyxp1QECgYEA8KcDkmwPrhqFlQe/U+Md27OhrQ4cecLCa6EVLsCXN1bFyCi3NSo2\n"
    "o5zFCC699KOL0ZwSmYlaQP4xjnqv4Gsa0s3uL7tqOJR2UuEtGK/MPMluGHVaWsGt\n"
    "zbnWH3xgsvvsxdt6hInFhcABLDupW336tJ8EcH7mOKoIP+azwF4kPiUCgYEA2c09\n"
    "zJBUW6SZXhgJ5vgENYc+UwDT7pfhIWZaRL+wXnwSoa7igodTKJtQp/KfFBJK4RA0\n"
    "prvwj4Wr/1ScaboR2hYZApbqXU5zkEkjC1hHIbg1fBe0EcnhP7ojMXrk6B5ed+Lq\n"
    "OVdYhUuvtdL/perelmbTJLnb8S214+tzVyg7EGkCgYEA6JLwX8zxpnhZSztOjBr9\n"
    "2zuSb7YojQBNd0kZOLLGMaQ5xwSactYWMi8rOIo76Lc6RFxKmXnl8NP5PtKRMRkx\n"
    "tjNxE05UDNRmOhkGxUn433JoZVjc9sMhXqZQKuPAbJoOLPW9RWQEsgtq1r3eId7x\n"
    "sSfRWYs6od6p1F/4rlwNOMUCgYEAtJmqf+DCAoe3IL3gICRSISy28k7CbZqE9JQR\n"
    "j+Y/Uemh7W29pyydOROoysq1PAh7DKrKbeNzcx8NYxh+5nCC8wrVzD7lsV8nFmJ+\n"
    "655UxVIhD3f8Oa/j1lr7acEU5KCiBtkjDU8vOMBsv+FpWOQrlB1JQa/X/+G+bHLF\n"
    "XmUerNkCgYEAv7R8vIKgJ1f69imgHdB31kue3wnOO/6NlfY3GTcaZcTdChY8SZ5B\n"
    "xits8xog0VcaxXhWlfO0hyCnZ9YRQbyDu0qp5eBU2p3qcE01x4ljJBZUOTweG06N\n"
    "cL9dYcwse5FhNMjrQ/OKv6B38SIXpoKQUtjgkaMtmpK8cXX1eqEMNkM=\n"
"-----END RSA PRIVATE KEY-----\n";

}

TEST_CASE( "Hello World" ) {
	try {
		// create a socket for the rep protocol
		nng::socket rep_sock = nng::rep::open();
	
		// rep starts listening using the tcp transport
		rep_sock.listen( "tcp://localhost:8000" );
	
		// create a socket for the req protocol
		nng::socket req_sock = nng::req::open();
	
		// req dials and establishes a connection
		req_sock.dial( "tcp://localhost:8000" );
	
		// req sends "hello" including the null terminator
		req_sock.send("hello");
	
		// rep receives a message
		nng::buffer rep_buf = rep_sock.recv();
	
		// check the content
		if( rep_buf == "hello" ) {
			// rep sends "world" in response
			rep_sock.send("world");
		}
	
		// req receives "world"
		nng::buffer req_buf = req_sock.recv();
	}
	catch( const nng::exception& e ) {
		// who() is the name of the nng function that produced the error
		// what() is a description of the error code
		printf( "%s: %s\n", e.who(), e.what() );
	}
}

TEST_CASE( "INPROC Socket", "[inproc]" ) {

	socket rep_socket = rep::open();
	rep_socket.listen( "inproc://inproc_test" );

	socket req_socket = req::open();
	req_socket.dial( "inproc://inproc_test" );

	req_socket.send("hello");
	REQUIRE( rep_socket.recv() == "hello" );
	
	rep_socket.send("world");
	REQUIRE( req_socket.recv() == "world" );
}

TEST_CASE( "IPC Socket", "[ipc]" ) {

	socket rep_socket = rep::open();
	rep_socket.listen( "ipc://ipc_test" );

	socket req_socket = req::open();
	req_socket.dial( "ipc://ipc_test" );

	req_socket.send("hello");
	REQUIRE( rep_socket.recv() == "hello" );
	
	rep_socket.send("world");
	REQUIRE( req_socket.recv() == "world" );
}

TEST_CASE( "TCP Socket", "[tcp]" ) {

	socket rep_socket = rep::open();
	rep_socket.listen( "tcp://localhost:8000" );

	socket req_socket = req::open();
	req_socket.dial( "tcp://localhost:8000" );

	req_socket.send("hello");
	REQUIRE( rep_socket.recv() == "hello" );
	
	rep_socket.send("world");
	REQUIRE( req_socket.recv() == "world" );
}

TEST_CASE( "TLS Socket", "[tls]" ) {

	tls::config server_cfg( tls::mode::server );
	server_cfg.config_own_cert(cert,key);

	socket rep_socket = rep::open();	
	listener listener( rep_socket.get(), "tls+tcp://localhost:8887" );
	set_opt_config( listener, server_cfg );
	listener.start();

	tls::config client_cfg( tls::mode::client );
	client_cfg.config_ca_chain(cert)
		.config_server_name("127.0.0.1")
		.config_auth_mode(tls::auth_mode::none);

	socket req_socket = req::open();
	dialer dialer( req_socket.get(), "tls+tcp://localhost:8887" );
	set_opt_config( dialer, client_cfg );
	dialer.start();

	req_socket.send("hello");
	REQUIRE( rep_socket.recv() == "hello" );
	
	rep_socket.send("world");
	REQUIRE( req_socket.recv() == "world" );
}

TEST_CASE( "WS Socket", "[ws]" ) {

	socket rep_socket = rep::open();
	rep_socket.listen( "ws://localhost:8000" );

	socket req_socket = req::open();
	req_socket.dial( "ws://localhost:8000" );

	req_socket.send("hello");
	REQUIRE( rep_socket.recv() == "hello" );
	
	rep_socket.send("world");
	REQUIRE( req_socket.recv() == "world" );
}

TEST_CASE( "WSS Socket", "[wss]" ) {

	tls::config server_cfg( tls::mode::server );
	server_cfg.config_own_cert(cert,key);

	socket rep_socket = rep::open();	
	listener listener( rep_socket.get(), "wss://localhost:8889" );
	set_opt_config( listener, server_cfg );
	listener.start();

	tls::config client_cfg( tls::mode::client );
	client_cfg.config_ca_chain(cert)
		.config_server_name("127.0.0.1")
		.config_auth_mode(tls::auth_mode::none);

	socket req_socket = req::open();
	dialer dialer( req_socket.get(), "wss://localhost:8889" );
	set_opt_config( dialer, client_cfg );
	dialer.start();

	req_socket.send("hello");
	REQUIRE( rep_socket.recv() == "hello" );
	
	rep_socket.send("world");
	REQUIRE( req_socket.recv() == "world" );


}