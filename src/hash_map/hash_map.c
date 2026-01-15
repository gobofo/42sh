#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *map = malloc(sizeof(struct hash_map));
    if (!map)
        return NULL;

    map->size = size;
    map->data = malloc(size * sizeof(struct pair_list *));
    if (!map->data)
    {
        free(map);
        return NULL;
    }

    for (size_t idx = 0; idx < size; idx++)
        map->data[idx] = NULL;

    return map;
}

bool hash_map_insert(struct hash_map *hash_map, char *key, char *value,
                     bool *updated)
{
    if (!hash_map || hash_map->size == 0 || !key)
        return false;

    size_t h = hash(key) % hash_map->size;

    struct pair_list *new_pair = malloc(sizeof(struct pair_list));
    if (!new_pair)
        return false;

    new_pair->key = strdup(key);
    new_pair->value = strdup(value);

    // No key in the table
    if (!hash_map->data[h])
    {
        new_pair->next = NULL;
    }
    else // The key already exists
    {
        // Check if key already exists
        struct pair_list *node = hash_map->data[h];
        while (node && strcmp(key, node->key) != 0)
            node = node->next;

        // No key found
        if (!node)
        {
            new_pair->next = hash_map->data[h];
        }
        else
        {
            hash_map->data[h]->value = strdup(value);

			free(new_pair->value);
			free(new_pair->key);
            free(new_pair);

			printf("Added key: %s with value: %s\n", key, value);

            if (updated)
                *updated = true;
            return true;
        }
    }

	printf("Added key: %s with value: %s\n", key, value);
    if (updated)
        *updated = false;
    hash_map->data[h] = new_pair;

    return true;
}

void hash_map_free(struct hash_map *hash_map)
{
    if (!hash_map)
        return;

    for (size_t idx = 0; idx < hash_map->size; idx++)
    {
        if (hash_map->data[idx])
        {
            // Free the linked list if necesarry
            struct pair_list *node = hash_map->data[idx];

            while (node)
            {
                struct pair_list *temp = node;
				
				free(temp->key);
				free(temp->value);

                node = node->next;
                free(temp);
            }
        }
    }

    free(hash_map->data);
    free(hash_map);
}

void hash_map_dump(struct hash_map *hash_map)
{
    if (!hash_map || hash_map->size == 0)
        return;

    for (size_t idx = 0; idx < hash_map->size; idx++)
    {
        if (hash_map->data[idx])
        {
            struct pair_list *node = hash_map->data[idx];

            while (node)
            {
                printf("%s: %s", node->key, node->value);

                // Not last element
                if (node->next)
                    printf(", ");

                node = node->next;
            }

            printf("\n");
        }
    }
}

char *hash_map_get(const struct hash_map *hash_map, char *key)
{
    if (!hash_map || hash_map->size == 0)
        return NULL;

    size_t h = hash(key);

    if (h > hash_map->size)
        h = h % hash_map->size;

    // A value exists for this hash
    if (hash_map->data[h])
    {
        struct pair_list *node = hash_map->data[h];

        while (node && strcmp(key, node->key) != 0)
            node = node->next;

        // Key not found
        if (!node)
            return NULL;

        return node->value;
    }

    return NULL;
}

bool hash_map_remove(struct hash_map *hash_map, char *key)
{
    if (!hash_map || hash_map->size == 0)
        return NULL;

    size_t h = hash(key) % hash_map->size;

    if (hash_map->data[h])
    {
        struct pair_list *node = hash_map->data[h];
        struct pair_list *prev = NULL;

        while (node && strcmp(key, node->key) != 0)
        {
            prev = node;
            node = node->next;
        }

        // No key found
        if (!node)
            return false;

        // First element of the list
        if (!prev)
        {
            hash_map->data[h] = node->next;

			free(node->key);
			free(node->value);
            free(node);
            return true;
        }

        prev->next = node->next;

		free(node->key);
		free(node->value);
		free(node);
    }
    else // Key does not exist at the current hash value
        return false;

    return true;
}

// int main(void)
//{
//      struct hash_map *map = hash_map_init(5);
//
//      bool updated;
//      hash_map_insert(map, "SKI", "winter", &updated);
//      hash_map_insert(map, "ACU", "51", &updated);
//      hash_map_insert(map, "C", "42", &updated);
//      hash_map_insert(map, "42", "life", &updated);
//      hash_map_insert(map, "SKI", "cold", &updated);
//
//      const char *res = hash_map_get(map, "S");
//      if (res)
//          printf("%s\n", res);
//
//      hash_map_dump(map);
//
//      printf("\n");
//
//      hash_map_insert(map, "S", "s", &updated);
//      hash_map_insert(map, "a", "to remove", &updated);
//      hash_map_dump(map);
//
//      printf("\n");
//
//      printf("Found: %i\n", hash_map_remove(map, "C"));
//      printf("Found: %i\n", hash_map_remove(map, "a"));
//      printf("Found: %i\n", hash_map_remove(map, "ACU"));
//      printf("Found: %i\n", hash_map_remove(map, "x"));
//      printf("Found: %i\n", hash_map_remove(map, "a"));
//
//      hash_map_dump(map);
//
//      hash_map_free(map);
//  }
