#ifndef INCLUDE_KAFKA_PRODUCER_H
#define INCLUDE_KAFKA_PRODUCER_H
#include "librdkafka/rdkafka.h"
#include <string>

struct KafkaControl {
  rd_kafka_t *control;
  rd_kafka_topic_t *topic;
};

class KafkaProducer {
 public:
  int Init(std::string brokers, std::string topic, std::string group); 
  
  int Send(char *buf, int len, int partition);

  int Send(char *buf, int len);

  static void MsgDelivered(rd_kafka_t *rk, void *payload, size_t len, rd_kafka_resp_err_t error_code, void *opaque, void *msg_opaque);

 private:

  static void Logger(const rd_kafka_t *rk,
                     int level,
                     const char *fac, 
                     const char *buf);
  std::string topic_;
  std::string server_;
  std::string group_;
  KafkaControl kafka_control_{NULL, NULL};
};
#endif
