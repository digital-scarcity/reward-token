#----------------------------
# Boilerplate
#----------------------------
# VERSION 1.1.0
# AUTHOR: Max Gravitt
# DESCRIPTION: base image of EOS dev

FROM eosio/eos-dev:v1.1.1
MAINTAINER Max Gravitt <gravitt@protonmail.com>
LABEL authors="gravitt@protonmail.com"

#COPY ./contracts-entrypoint.sh /
#COPY ./contracts /opt/eosio/bin/data-dir/contracts

#RUN chmod +x ./contracts-entrypoint.sh
