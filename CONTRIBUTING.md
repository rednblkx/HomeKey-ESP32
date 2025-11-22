# Contributing to HomeKey-ESP32

Thank you for your interest in contributing to HomeKey-ESP32! We welcome contributions from the community and are pleased to have you join us.

## How to Contribute

### Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone --recursive https://github.com/your-username/HomeKey-ESP32.git
   cd HomeKey-ESP32
   ```

### Development Setup

1. **Install ESP-IDF** (see the [ESP-IDF Installation Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#what-you-need) for details):

2. **Install web development dependencies**:
   ```bash
   cd data
   ## You need Bun instead, feel free to use npm or yarn instead if you prefer
   bun install
   ```

3. **Update submodules**:
   ```bash
   git submodule update --init --recursive
   ```

### Making Changes

1. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** following our coding standards (see below)

3. **Test your changes**:
   - For ESP32 firmware: `idf.py build && idf.py flash monitor`
   - For web interface: `cd data && npm run dev`

4. **Commit your changes**:
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   ```

5. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create a Pull Request** on GitHub

### Pull Request Process

1. Title your PR with a clear and descriptive title
2. Provide a detailed description of your changes and the problem you're trying to solve
3. **Request review** from maintainers
4. **Address review feedback** promptly
5. **Squash commits** if requested

## Types of Contributions

### Bug Reports

- Include steps to reproduce
- Provide system information
- Include relevant logs or screenshots

### Feature Requests

- Explain the use case
- Describe the expected behavior
- Consider implementation complexity

### Code Contributions

- **Firmware**: ESP32 C++ code in `main/`
- **Web Interface**: Svelte code in `data/`
- **Documentation**: Markdown files in `docs/`

### Documentation

- Fix typos and improve clarity
- Add missing documentation
- Update outdated information

### Testing

- Test on different hardware configurations
- Report compatibility issues
- Validate new features
- Performance benchmarking

## Coding Standards

### C++ Code (ESP32 Firmware)

```cpp
// Use consistent indentation (2 spaces)
// Use meaningful variable names
// Add comments for complex logic

class ExampleClass {
public:
  void exampleMethod();
  
private:
  int m_exampleVariable;
};
```

### Commit Messages

Follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test changes
- `chore`: Maintenance tasks

Examples:
```
feat(nfc): add support for multiple card types
fix(web): resolve configuration save issue
docs: update installation guide for Windows
```

## Testing Guidelines

### Integration Tests

- Verify MQTT functionality
- Test HomeKit integration
- Validate web interface

### Hardware Compatibility

Test on different ESP32 variants:
- ESP32-WROOM-32
- ESP32-S3
- ESP32-C3
- ESP32-C6

## Security Guidelines

### Code Security

- Validate all inputs
- Use secure random number generation
- Implement proper error handling
- Avoid hardcoded secrets

### Cryptographic Operations

- Use established libraries
- Follow cryptographic best practices
- Secure key storage
- Proper random seeding

### Network Security

- Use TLS for sensitive data
- Validate certificates
- Implement proper authentication
- Avoid information leakage

## Issue Templates

### Bug Report Template

```markdown
**Describe the bug**
A clear description of the bug.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '...'
3. See error

**Expected behavior**
What you expected to happen.

**Screenshots**
If applicable, add screenshots.

**Environment:**
- Hardware: [e.g., ESP32-WROOM-32]
- Firmware version: [e.g., 1.0.0]
- Web UI version: [e.g., 1.0.0]

**Additional context**
Add any other context about the problem here.
```

### Feature Request Template

```markdown
**Is your feature request related to a problem?**
A clear description of the problem.

**Describe the solution you'd like**
A clear description of what you want to happen.

**Describe alternatives you've considered**
A clear description of any alternative solutions you've considered.

**Additional context**
Add any other context or screenshots about the feature request here.
```

## Getting Help

### Before Asking

1. Check existing issues and discussions
2. Read the documentation thoroughly
3. Search closed issues for similar problems
4. Test with the latest development version

### How to Ask

1. Use the appropriate template
2. Provide complete information
3. Include relevant logs and screenshots
4. Be specific about your environment
5. Describe what you've already tried

### Community Resources

- **Discord**: [Join our server](https://discord.com/invite/VWpZ5YyUcm)
- **GitHub Discussions**: For general questions and ideas
- **GitHub Issues**: For bug reports and feature requests
- **Documentation**: [Read the docs](https://rednblkx.github.io/HomeKey-ESP32/)

## Recognition

Contributors will be:
- **Given credit** in release notes
- **Invited to the contributors team** after significant contributions

## 📜 Legal

### License

By contributing, you agree that your contributions will be licensed under the same MIT License that covers the project.

### Copyright

You retain copyright to your contributions. We ask that you license them under the MIT license so they can be included in the main project.

---

Thank you for contributing to HomeKey-ESP32! Your efforts help make smart home technology more accessible to everyone.
