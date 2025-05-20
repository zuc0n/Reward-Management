# RewardManagement

A C++ CLI application for user and wallet management with file-based storage, authentication, and 2FA.

## Contributors
- Pham Thanh Cong - K24DTCN504
- Tran Cong Ty - K24DTCN247

## Overview

- Client (CLI)
- API Layer
- Authentication Layer
- Business Logic Layer
- File I/O Layer
- Persistence Layer

## Dependencies

- C++17
- nlohmann/json
- OpenSSL

## Build

```
mkdir build && cd build
cmake ..
make
```

## Usage

```
./RewardManagement
```

## Directory Structure

```
include/
  models/
    UserAccount.h
    Wallet.h
    Transaction.h
  storage/
    FileManager.h
    UserStorage.h
  auth/
    AuthService.h
  services/
    UserService.h
    OTPService.h
    WalletService.h
    AdminService.h
  api/
    ApiResponse.h
    ApiRouter.h
  client/
    CLIClient.h

src/
  models/
  storage/
  auth/
  services/
  api/
  client/

CMakeLists.txt
README.md
```

## Data Directories

- data/users
- data/wallets
- data/sessions

Create them before running:

```
mkdir -p data/users data/wallets data/sessions
``` 
