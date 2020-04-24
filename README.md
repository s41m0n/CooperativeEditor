# CooperativeEditor

*System and Device Programming project* - Cooperative multiuser editor.

The aim of this project is to implement, using client-server architecture, a C++ cooperative editor thanks to many user can work on the same file.

## Dependencies

The following libraries need to be installed in your system:

* Qt (>= 5.12)
* sqlite3

## Usage

To launch the CooperativeServer: `./CooperativeServer <local_port> [-d]`

To launch the CooperativeClient: `./CooperativeClient <server_address> <server_port> [-d]`

