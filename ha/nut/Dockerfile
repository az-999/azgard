FROM dockerhub.timeweb.cloud/library/debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        nut \
        nut-client \
        nut-server \
        usbutils \
    && rm -rf /var/lib/apt/lists/*

COPY run-nut.sh /usr/local/bin/run-nut.sh
RUN chmod +x /usr/local/bin/run-nut.sh

EXPOSE 3493

CMD ["/usr/local/bin/run-nut.sh"]
