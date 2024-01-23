#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ubject.h>

#include "Var.h"
#include "Var.r.h"

#define LAUD_SHUFFLE_PRIORITY (LAUD_VAR_PRIORITY + 2)
#define SHUFFLE_C

struct LaudShuffle {
#ifndef SHUFFLE_C
  char ___[sizeof(struct ___ {
#endif
    /**< Base LaudVar structure. */
    struct LaudVar _;
    /**< Pointer to the dependency LaudVar. */
    struct LaudVar *dependency;
    /**< 2D array representing shuffles for each dimension. */
    size_t **shuffles;
    /**< Array storing the number of effective swaps per dimension.
     */
    size_t *number_of_effective_swaps_per_dim;
    /**< Number of dimensions to shuffle. */
    size_t dims_to_shuffle;

#ifndef SHUFFLE_C
  })];
#endif
};
#undef SHUFFLE_C

const void *LaudShuffle;

#ifdef USE_CHAR_MARKER_FOR_SHUFFLE
typedef char marker_t;
#else
struct marker {
  char marker : 1;
};
typedef struct marker marker_t;
#endif
static inline char is_marked(marker_t *marker) {
  return (*(char *)marker) == 1;
}
static inline char mark(marker_t *marker) { return *(char *)marker = 1; }

static inline void perform_shuffle_recursively(
    const size_t dims_to_shuffle, const size_t rank, const size_t *const shape,
    const size_t dim, const size_t dim_multiplier, size_t *n_swaps,
    size_t **shuffle_swaps, const float *const values, float *shuffled_values);

size_t *create_shuffle_swaps(size_t dim_size, size_t *n_swaps_prepared) {
  // Calculate the number of swaps to prepare
  *n_swaps_prepared = dim_size / 2 + 1;

  // Allocate memory for swaps array
  size_t *swaps = malloc(*n_swaps_prepared * sizeof(size_t));

  if (!swaps) {
    // Handle memory allocation failure for swaps
    UbjectError.error(
        "create_shuffle_swaps: memory allocation failed for swaps\n");
    return NULL;
  }

  // Allocate memory for is_used array
  size_t *is_used = calloc(dim_size, sizeof(size_t));

  if (!is_used) {
    // Handle memory allocation failure for is_used
    free(swaps);
    UbjectError.error(
        "create_shuffle_swaps: memory allocation failed for use logger\n");
    return NULL;
  }

  // Generate random swaps
  for (size_t i = 0; i < *n_swaps_prepared; ++i) {
    // Generate a random index using laud_rng()
    size_t random_index = (size_t)dim_size * laud_rng();

    // Find the next unused random index
    while ((is_used[random_index])) {
      random_index = (random_index + 1) % dim_size;
    }

    // Store the random index as a swap
    swaps[i] = random_index;
    is_used[random_index] = 1; // Mark the index as used
  }

  // Free memory used for is_used array
  free(is_used);

  return swaps;
}

static inline void copy_value_to_shuffled(
    const size_t dims_to_shuffle, const size_t rank, const size_t *const shape,
    const size_t dim, const size_t dim_multiplier, size_t *n_next_dim_swaps,
    size_t **next_shuffle_swaps, marker_t *is_used, const size_t i,
    const size_t j, const float *values, float *shuffled_values) {

  // Check if the current dimension is the last dimension to shuffle
  if (rank == (dim + 1)) {
    // Directly copy the value to the shuffled_values array
    shuffled_values[i] = values[j];
  } else {
    // Calculate pointers for the relevant section of values and shuffled_values
    float *shuffled_values_of_interest = shuffled_values + (i * dim_multiplier);
    const float *values_of_interest = values + (j * dim_multiplier);

    // Check if the next dimension should be shuffled
    if (dims_to_shuffle >> (dim + 1)) {
      // If yes, perform shuffle recursively for the next dimension
      size_t next_dim = dim + 1;
      size_t next_dim_multiplier = dim_multiplier / shape[next_dim];

      perform_shuffle_recursively(dims_to_shuffle, rank, shape, next_dim,
                                  next_dim_multiplier, n_next_dim_swaps,
                                  next_shuffle_swaps, values_of_interest,
                                  shuffled_values_of_interest);
    } else {
      // If not, directly copy the values to the shuffled_values array
      memcpy(shuffled_values_of_interest, values_of_interest,
             sizeof(float) * dim_multiplier);
    }
  }

  // Mark the index as used
  mark(&is_used[j]);
}

static inline void perform_shuffle_recursively(
    const size_t dims_to_shuffle, const size_t rank, const size_t *const shape,
    const size_t dim, const size_t dim_multiplier, size_t *n_shuffle_swaps,
    size_t **shuffle_swaps, const float *const values, float *shuffled_values) {

  // Allocate memory to track whether an index is used
  marker_t *is_used = calloc(shape[dim], sizeof(marker_t));
  size_t i = 0;

  // Check if the current dimension needs shuffling
  if (dims_to_shuffle & (1 << dim)) {
    size_t *effective_swaps;
    size_t effective_n_swaps;

    // Check if shuffle_swaps is provided or needs to be created
    if (!shuffle_swaps || !*shuffle_swaps) {
      effective_swaps = create_shuffle_swaps(shape[dim], &effective_n_swaps);

      // Update shuffle_swaps and n_shuffle_swaps if provided
      if (shuffle_swaps) {
        *shuffle_swaps = effective_swaps;
      }
      *n_shuffle_swaps = effective_n_swaps;
    } else {
      // Use the provided shuffle_swaps and n_shuffle_swaps
      effective_swaps = *shuffle_swaps;
      effective_n_swaps = *n_shuffle_swaps;
    }

    // Move pointers to the next set of swaps
    shuffle_swaps++;
    n_shuffle_swaps++;

    // Perform shuffling for each swap
    for (i = 0; i < effective_n_swaps; i++) {
      copy_value_to_shuffled(dims_to_shuffle, rank, shape, dim, dim_multiplier,
                             n_shuffle_swaps, shuffle_swaps, is_used, i,
                             effective_swaps[i], values, shuffled_values);
    }
  } else {
    //*n_swaps = 0;
  }

  // Process remaining unused indices
  for (size_t j = 0; j < shape[dim]; j++) {
    if (!is_marked(&is_used[j])) {
      copy_value_to_shuffled(dims_to_shuffle, rank, shape, dim, dim_multiplier,
                             n_shuffle_swaps, shuffle_swaps, is_used, i, j,
                             values, shuffled_values);
      i++;
    }
  }

  // Free the memory allocated for is_used
  free(is_used);
}

static const void *laud_shuffle_evaluate(void *self) {
  // Cast the input pointer to the appropriate type
  struct LaudShuffle *shuffle_instance = self;

  // Obtain values from the dependency
  const float *const x_values = laud_values(shuffle_instance->dependency);
  // shuffle_dim(x, laud_values(shuffled_x), 0, n_dim, dims);

  // Get the shape of the shuffle_instance
  const size_t *shape = laud_shape(self);

  // Perform recursive shuffling
  perform_shuffle_recursively(
      shuffle_instance->dims_to_shuffle,
      laud_rank(shuffle_instance->dependency), shape, 0,
      laud_length(shuffle_instance->dependency) / shape[0],
      shuffle_instance->number_of_effective_swaps_per_dim,
      shuffle_instance->shuffles, x_values,
      (float *)laud_values(shuffle_instance));

  // Return the shuffled instance
  return shuffle_instance;
}

static const void *laud_shuffle_differentiate(void *self) {
  struct LaudShuffle *shuffle_instance = self;
  UbjectError.warn("No differentiation implemented for %p\n", shuffle_instance);
  return NULL;
}

void *laud_shuffle(const void *input_var, size_t num_dims_to_shuffle,
                   size_t *dims) {
  // Create a new LaudShuffle variable
  struct LaudShuffle *shuffled_var =
      init(LaudShuffle, laud_rank(input_var), laud_shape(input_var), 0, NULL);

  // Check if there are dimensions to shuffle
  if (num_dims_to_shuffle) {
    // Initialize dimensions to shuffle in the LaudShuffle variable
    shuffled_var->dims_to_shuffle = 0;
    for (size_t i = 0; i < num_dims_to_shuffle; i++) {
      shuffled_var->dims_to_shuffle |= 1 << dims[i];
    }

    // Allocate memory for storing shuffle information
    shuffled_var->shuffles = calloc(num_dims_to_shuffle, sizeof(void *));
  }

  // Set the dependency of the shuffled variable to the input variable
  shuffled_var->dependency = (void *)input_var;

  // Allocate memory for storing the number of effective swaps per dimension
  shuffled_var->number_of_effective_swaps_per_dim =
      malloc(num_dims_to_shuffle * sizeof(size_t));

  // Return the created LaudShuffle variable
  return shuffled_var;
}

void *laud_shuffle_like(const void *input_var,
                        const void *shuffled_var_template) {
  // Extract information from the template LaudShuffle variable
  const struct LaudShuffle *template_shuffle = shuffled_var_template;

  // Get the rank of the input variable
  size_t rank = laud_rank(input_var);

  // Create a new LaudShuffle variable
  struct LaudShuffle *shuffled_var =
      init(LaudShuffle, rank, laud_shape(input_var), 0, NULL);

  // Set dimensions to shuffle based on the template, considering rank
  shuffled_var->dims_to_shuffle =
      template_shuffle->dims_to_shuffle & (size_t) ~((~0x0) << rank);

  // Allocate memory for storing shuffle information
  shuffled_var->shuffles = calloc(rank, sizeof(size_t *));
  shuffled_var->number_of_effective_swaps_per_dim =
      malloc(rank * sizeof(size_t));

  // Copy shuffle information from the template to the new variable
  size_t i = 0;
  while ((shuffled_var->dims_to_shuffle >> i) && (i < rank)) {
    shuffled_var->number_of_effective_swaps_per_dim[i] =
        template_shuffle->number_of_effective_swaps_per_dim[i];

    // Allocate and copy shuffle swaps
    shuffled_var->shuffles[i] = malloc(
        shuffled_var->number_of_effective_swaps_per_dim[i] * sizeof(float));
    memcpy(shuffled_var->shuffles[i], template_shuffle->shuffles[i],
           shuffled_var->number_of_effective_swaps_per_dim[i] * sizeof(float));
    i++;
  }

  // Set the dependency of the shuffled variable to the input variable
  shuffled_var->dependency = (void *)input_var;

  // Return the created LaudShuffle variable
  return shuffled_var;
}

const void *LaudShuffle = NULL;
static void __attribute__((constructor(LAUD_SHUFFLE_PRIORITY)))
initLaudShuffle(void) {
  if (!LaudShuffle) {
    LaudShuffle =
        init(LaudVarClass, LaudVar, sizeof(struct LaudShuffle), className,
             "LaudShuffle", laud_evaluate, laud_shuffle_evaluate,
             laud_differentiate, laud_shuffle_differentiate, NULL);
  }
}
