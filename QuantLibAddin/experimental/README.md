python3 -m venv venv_with_quantlib
source venv_with_quantlib/bin/activate

pip install pip-tools
pip-compile requirements.in

pip install -r requirements.txt

export PYTHON_SITE_PACKAGES=venv_with_quantlib/lib/python3.9/site-packages

mkdir build
cd build
cmake ..
make



./options

./proxy_pricer
