# Build and Run Instructions

## 1. Configure and Build the Project

> Note: There is no mention of QuantLib in the CMake configuration.

```bash
mkdir build
cd build

cmake ..

make

```

## 2. Create and Configure the Python Environment (inside the cmake build directory)

```bash
python3 -m venv venv_with_quantlib

source venv_with_quantlib/bin/activate

pip install pip-tools

pip-compile requirements.in

pip install -r requirements.txt
```
---

## 3. Run the Examples

> Note: Update the Python version (`3.9`) if using a different interpreter version.

```bash
export PYTHON_SITE_PACKAGES=venv_with_quantlib/lib/python3.9/site-packages
```

### Options Pricer
```bash
./options
```

Runs the options pricing example.

### Proxy Pricer

```bash
./proxy_pricer
```

Runs the proxy pricing example.
