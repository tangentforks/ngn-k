/ K3 IPC - Inter-Process Communication
/ Kona-compatible wire format (64-bit header)
/
/ In-memory serialization:
/   ` 3: value        / serialize to bytes
/   ` 4: bytes        / deserialize from bytes
/
/ Client (network send):
/   (`;port) 3: value       / async send to localhost:port
/   (`host;port) 3: value   / async send to host:port
/   (`;port) 4: value       / sync send, wait for response
/   (`host;port) 4: value   / sync send to host:port
/
/ Server (command line):
/   ./k -l 1234             / start listening on port 1234
/   .m.s:{[x] ...}          / async message handler (3: messages)
/   .m.g:{[x] ...}          / sync handler (4: messages, return sent back)
/
/ Examples:
/   ` 4: ` 3: 1 2 3         / roundtrip: serialize then deserialize
/   (`;5555) 3: "hello"     / send async message to localhost:5555
/   (`;5555) 4: `query      / send sync, get response
/
/ Helpers:
ser:` 3:                    / serialize value to bytes
des:` 4:                    / deserialize bytes to value

/ TODO (remaining work):
/ - Test IPC between two ngn-k instances (server and client)
/ - Test IPC interop with Kona
/ - The REPL thread approach on Windows may have issues:
/   - When running with -l and a script, stdin reads go through socket pair
/   - The ipc_stdin_read() function routes stdin through repl_sock[0]
/   - Need to verify 1:` works correctly in server mode scripts
/ - Connection caching: connections are cached by (host,port) but never closed
/   - May need explicit close or timeout mechanism
/ - Error handling could be improved:
/   - Currently returns 'io error on connection failure
/   - Could provide more specific error messages
/ - Handler dispatch (.m.s/.m.g) uses gg() which may have issues
/   - Need to test handler invocation thoroughly
/ - No authentication or encryption (same as K3/Kona)
