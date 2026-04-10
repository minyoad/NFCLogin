---
name: "windows-credential-provider"
description: "Develops Windows Credential Provider for system-level NFC login integration. Invoke when user needs Windows login screen NFC card support or credential provider development."
---

# Windows Credential Provider Developer

This skill specializes in developing Windows Credential Provider components for system-level login integration with NFC card authentication.

## Core Capabilities

### Credential Provider Development
- COM component implementation (ICredentialProvider interface)
- C++ development for system-level integration
- Registry registration/unregistration
- DLL compilation and deployment
- Windows 10/11 compatibility

### NFC Integration Features
- MIFARE Classic (M1) card UID reading
- Local Windows account binding
- New local user creation
- UID-to-account mapping database
- Real-time NFC card detection

### Security Implementation
- Secure credential serialization
- Local account authentication
- UID validation and verification
- System-level privilege management
- Credential provider isolation

## Development Workflow

1. **Interface Implementation**
   - Implement ICredentialProvider interface
   - Create credential tiles
   - Handle user interaction events
   - Serialize credentials for Windows

2. **NFC Hardware Integration**
   - Serial communication setup
   - M1 card UID extraction
   - Anti-collision handling
   - Error recovery mechanisms

3. **Account Management**
   - Local user account lookup
   - UID-to-account mapping
   - New account creation
   - Account status validation

4. **System Integration**
   - Registry registration
   - COM registration
   - Windows logon process integration
   - Credential provider ordering

## Key Technical Components

### Credential Provider Interface
```cpp
class NFCCredentialProvider : public ICredentialProvider {
    // Implementation for tile management
    // User credential collection
    // Authentication serialization
};
```

### NFC Card Reading
```cpp
std::string ReadNFCCardUID() {
    // M1 card anti-collision
    // UID extraction
    // Validation checks
}
```

### Account Database
```cpp
class AccountManager {
    // Local account lookup by UID
    // New account creation
    // Account validation
    // Mapping management
};
```

## Build and Deployment

### Requirements
- Visual Studio 2022 or later
- Windows SDK 10.0.22000.0+
- C++ development tools
- Administrator privileges

### Build Process
```bash
# Build the credential provider DLL
msbuild NFCCredentialProvider.sln /p:Configuration=Release /p:Platform=x64

# Register the provider
regsvr32 NFCCredentialProvider.dll

# Test the integration
# Lock Windows and verify NFC login appears
```

## Testing and Validation

### Test Scenarios
- Clean Windows installation
- Existing local accounts
- New account creation
- NFC card registration
- Login with NFC card
- Fallback to password

### Debug Support
- Comprehensive logging
- Event viewer integration
- Registry state inspection
- COM registration verification

## Common Issues and Solutions

### Registration Failures
- Check administrator privileges
- Verify COM registration
- Validate registry entries
- Test with Process Monitor

### NFC Reading Issues
- Serial port configuration
- Card compatibility (M1 only)
- Anti-collision timing
- Hardware driver issues

### Authentication Problems
- Account status validation
- UID mapping accuracy
- Credential serialization
- Windows logon integration

## Best Practices

### Security
- Never store passwords in provider
- Use Windows authentication APIs
- Validate all inputs
- Implement proper error handling

### Performance
- Minimize UI blocking
- Cache account information
- Optimize NFC reading
- Handle concurrent requests

### Maintainability
- Comprehensive logging
- Modular code structure
- Clear error messages
- Version management

This skill is essential for transforming the current application-level NFC login into a seamless Windows system-level authentication experience.