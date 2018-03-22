#ifndef INCLUDE_CAPTURE_H
#define INCLUDE_CAPTURE_H
#include "kafka_producer.h"

int Demo_Capture();

class KafkaService {
 public:
  static KafkaProducer* getKafkaProducer() {
    static KafkaProducer instance;
    return &instance;
  }
};
#endif
