/ K3 IPC - Inter-Process Communication
/ Serialization/deserialization compatible with K3 protocol
/
/ Usage:
/   b:(3:)[0;x]     / serialize x to bytes (in-memory)
/   fd(3:)x         / serialize and send x over socket fd
/   fd(4:)()        / receive from socket fd
/
/ Note: 4: has parser issues with variable arguments.
/ Use literals or inline expressions for deserialization.
/
/ Network:
/   fd:<"host:port" / open connection
/   >fd             / close connection
/
/ Examples:
/   b:(3:)[0;123]        / serialize integer
/   b:(3:)[0;"hello"]    / serialize string
/   b:(3:)[0;`a`b`c]     / serialize symbols

/ Serialize value to K3 IPC bytes (in-memory mode with fd=0)
ser:(3:)[0;]

/ Send value over socket fd (network mode)
snd:{x(3:)y}

/ Receive value from socket fd
rcv:{x(4:)()}

