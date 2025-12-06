#!k
/ [this code was generated and tested via claude, and is in the public domain]
/ MCP (Model Context Protocol) server for ngn/k
/ Implements JSON-RPC 2.0 over stdio
/ Reads requests from stdin and responds on stdout
/ Note: Requires persistent stdin connection (as provided by MCP clients)
/ Installation in Claude Code (experimental):
/ Option 1: Using claude mcp add command
/   claude mcp add ngnk /path/to/ngnk/k /path/to/ngnk/mcp.k
/ Option 2: Manual JSON configuration
/   Edit ~/.claude/config.json and add ngnk server with command /path/to/k
/   and args /path/to/mcp.k
/ Testing:
/   echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{}}' | ./k mcp.k
/   cat requests.jsonl | ./k mcp.k

\d mcp

/ JSON-RPC response builders
ok:{`jsonrpc`id`result!("2.0";x;y)}
err:{`jsonrpc`id`error!("2.0";x;`code`message!(y;z))}

/ MCP method handlers
initialize:{[id;params]
 caps:`tools`prompts!(`listChanged!(::);`listChanged!(::))
 info:`name`version!("ngnk";"1.0.0")
 result:`protocolVersion`capabilities`serverInfo!("2024-11-05";caps;info)
 ok[id;result]}

toolslist:{[id;params]
 codeprop:`type`description!("string";"K code to execute")
 schema:`type`properties`required!("object";(,`code)!,codeprop;,"code")
 tool:`name`description`inputSchema!("execute_k";"Execute K code and return the result";schema)
 ok[id;`tools!,,tool]}

toolscall:{[id;params]
 args:params`arguments
 code:args`code
 result:.[.:;,code;{`error`message!("eval_error";x)}]
 text:`k@result
 $[`error~@result;err[id;-1;result`message];ok[id;(,`content)!,,(`type`text!("text";text))]]}

/ Method dispatcher
dispatch:{[req]
 id:req`id
 method:req`method
 params:req`params
 $[method~"initialize";initialize[id;params]
   method~"tools/list";toolslist[id;params]
   method~"tools/call";toolscall[id;params]
   err[id;-32601;"Method not found"]]}

/ Process one line
proc:{[line]
 req:`j?line
 resp:dispatch[req]
 `0:,`j@resp
 0}

/ Entry point - read stdin line by line and process
g:0
run:{{s:0:0;$[#s;[proc's;g+::1];0]}::/0}

\d .

/ Start server
mcp.run[]
