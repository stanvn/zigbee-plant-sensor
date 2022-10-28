#include "adc.hpp"

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

const struct adc_dt_spec adc_c::m_adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

adc_c::adc_c(uint16_t channel){
  m_sequence = {
		.buffer = &m_buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(m_buf),
    .calibrate = true
	};
  m_adc_spec = m_adc_channels[channel];
}

/**
* @brief Initializes the adc
* @return STATUS_SUCCESS on success
* @return Error code when init failed
**/
status_code_t adc_c::init(){
  status_code_t st;
  if(!device_is_ready(m_adc_spec.dev)){
    return STATUS_ERROR_DEVICE_NOT_READY;
  }
  st = adc_channel_setup_dt(&m_adc_spec);
  return -st;
}


/**
* @brief Read value from the adc
* @return integer >= 0 containing the measurement value
* @return integer < 0 with a negative error code on failure
**/
int32_t adc_c::read(){
  int32_t st;
  int32_t val_mv;
  (void)adc_sequence_init_dt(&m_adc_spec, &m_sequence);
  st = adc_read(m_adc_spec.dev, &m_sequence);
  if(st < 0){
    return st;
  }
  val_mv = m_buf;
  st = adc_raw_to_millivolts_dt(&m_adc_spec, &val_mv);
  if(st < 0){
    return st;
  }
  return val_mv;
}
