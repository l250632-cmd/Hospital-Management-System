# Makefile for MediCore Hospital Management System
# Requires SFML 2.6.2 installed
# On Ubuntu/Debian: sudo apt install libsfml-dev
# On Windows with SFML downloaded: adjust SFML_PATH below

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = MediCore

# Linux - SFML installed via package manager
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

SOURCES = main.cpp
HEADERS = HospitalException.h MyString.h Person.h Patient.h Doctor.h Admin.h \
          Appointment.h Bill.h Prescription.h Storage.h FileHandler.h \
          Validator.h HospitalSystem.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(SFML_LIBS)

clean:
	rm -f $(TARGET)

# Windows (MinGW) with SFML 2.6.2 downloaded to C:\SFML-2.6.2
# Uncomment and adjust:
# win:
# 	$(CXX) $(CXXFLAGS) -IC:\SFML-2.6.2\include -o $(TARGET).exe $(SOURCES) \
# 	  -LC:\SFML-2.6.2\lib -lsfml-graphics -lsfml-window -lsfml-system \
# 	  -mwindows
