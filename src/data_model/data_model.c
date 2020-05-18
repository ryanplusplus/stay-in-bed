/*!
 * @file
 * @brief
 */

#include <stddef.h>
#include "data_model.h"
#include "data_model_key_value_pairs.h"
#include "tiny_utils.h"

#define _storage_type_name data_model_storage_t
generate_storage_type_for_ram_key_value_pairs(data_model_key_value_pairs);

static struct {
  data_model_storage_t storage;
  tiny_ram_key_value_store_t key_value_store;
} self;

static const tiny_ram_key_value_store_key_value_pair_t key_value_pairs[] = {
  generate_configuration_pairs_from_ram_key_value_pairs(data_model_key_value_pairs)
};

static const tiny_ram_key_value_store_configuration_t configuration = {
  key_value_pairs,
  element_count(key_value_pairs)
};

void data_model_init(void) {
  tiny_ram_key_value_store_init(&self.key_value_store, &configuration, &self.storage);
}

i_tiny_key_value_store_t* data_model_key_value_store(void) {
  return &self.key_value_store.interface;
}
