#include "kafka_producer.h"
#include "sys/time.h"
#include "error.h"
#include "librdkafka/rdkafka.h"
#include "glog/logging.h"
//#include <thread>

int KafkaProducer::Init(std::string brokers, std::string topic, std::string group) {
    char errstr[512];
    rd_kafka_topic_conf_t *topic_conf;
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    rd_kafka_conf_set_log_cb(conf, Logger);
    rd_kafka_conf_set_dr_cb(conf, MsgDelivered);
    if (RD_KAFKA_CONF_OK != rd_kafka_conf_set(conf, "group.id", group.c_str(), NULL, 0))
      return -2;
    rd_kafka_conf_set(conf, "queued.min.messages", "1000000", NULL, 0);	
    /* Create Kafka handle */
    if (!(kafka_control_.control = rd_kafka_new(RD_KAFKA_PRODUCER, conf,
	                    errstr, sizeof(errstr)))) {
      fprintf(stderr, "%% Failed to create new producer: %s\n",
				errstr);
      return -1;
    }
    /* Add brokers */
    if (rd_kafka_brokers_add(kafka_control_.control, brokers.c_str()) == 0) {
      return -2;
    }
    topic_conf = rd_kafka_topic_conf_new();
    /* Create topic */
    kafka_control_.topic = rd_kafka_topic_new(kafka_control_.control, topic.c_str(), topic_conf);
    topic_conf = NULL; /* Now owned by topic */
    rd_kafka_dump(stdout, kafka_control_.control);
    printf("create rd kafka ok\n");
    return 0;    
}  
 
int KafkaProducer::Send(char *buf, int len) {
  Send(buf, len, RD_KAFKA_PARTITION_UA);
  return 0;
}

int KafkaProducer::Send(char *buf, int len, int partition) {
    /* Start consuming */
  int rc = 0;
  if (kafka_control_.topic == NULL)
    return -1;
  /* Send/Produce message. */
  if (rd_kafka_produce(kafka_control_.topic, 
                       partition,
		       RD_KAFKA_MSG_F_COPY,
		       /* Payload and length */
		       buf, 
                       len,
		       /* Optional key and its length */
		       NULL, 
                       0,
		       NULL) == -1) {
      char log_buf[512];
      snprintf(log_buf, 
               sizeof(log_buf),
	       "%% Failed to produce to topic %s"
	       "partition %i: %s\n",
		rd_kafka_topic_name(kafka_control_.topic), partition,
		rd_kafka_err2str(rd_kafka_errno2err(errno)));
      /* Poll to handle delivery reports */
      rc = -1;
      LOG(ERROR) << log_buf;
  }
  rd_kafka_poll(kafka_control_.control, 0);
  return 0;
}

void KafkaProducer::MsgDelivered (rd_kafka_t *rk,
			          void *payload, 
                                  size_t len,
			          rd_kafka_resp_err_t error_code,
			          void *opaque, 
                                  void *msg_opaque) {

  if (error_code)
    LOG(ERROR) <<  "Message delivery failed:" << rd_kafka_err2str(error_code);
}

void KafkaProducer::Logger(const rd_kafka_t *rk,
                     int level,
                     const char *fac, 
                     const char *buf) {
struct timeval tv;
	gettimeofday(&tv, NULL);
        char log_buf[512];
	snprintf(log_buf, sizeof(log_buf), "%u.%03u RDKAFKA-%i-%s: %s: %s\n",
		(int)tv.tv_sec, (int)(tv.tv_usec / 1000),
		level, fac, rk ? rd_kafka_name(rk) : NULL, buf);
        LOG(INFO) << log_buf;
}
