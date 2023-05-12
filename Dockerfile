FROM ubuntu

#RUN apt-get update && apt-get install build-essential
WORKDIR	/tmp

COPY ./Makefile .
COPY ./inc .
COPY ./source .
