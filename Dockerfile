FROM mfreudenberg/arduino-builder:0.1.1
COPY arduino-builder-config.yml ./
RUN [ "/venv/bin/python", "arduino-cli-config.py", "--no-compile" ]
