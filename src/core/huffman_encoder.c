#include "huffman.h"
#include "huffman_internal.h"
#include "types_errors.h"
#include "logger.h"
#include "bytes_manager.h"

#include <stdlib.h>
#include <stdio.h>

static Status count_char_frequency(Huffman_Encoder *encoder)
{
    log_message(LOG_DEBUG, "Counting character frequencies");
    Bytes_Reader *reader = encoder->reader;
    unsigned char letter;

    Status status = STATUS_OK;
    while (status == STATUS_OK)
    {
        status = read_byte_from_file(&letter, reader);
        if (status != STATUS_OK && status != ERROR_END_OF_FILE)
            return status;
        if (status == ERROR_END_OF_FILE)
            break;
        encoder->char_frequency[letter]++;
    }
    return STATUS_OK;
}

static void insert_sorted_node(Frequency_Node *new_node, Huffman_Encoder *encoder)
{
    if (encoder->frequency_list->first_node == NULL){
        encoder->frequency_list->first_node = new_node;
        encoder->frequency_list->size++;
        return;
    }

    Frequency_Node *next_node = encoder->frequency_list->first_node;
    Frequency_Node *prev_node = NULL;
    for (int i = 0; i < encoder->frequency_list->size + 1; i++)
    {
        if (next_node->next == NULL && prev_node)
        {
            if (new_node->frequency >= next_node->frequency)
                next_node->next = new_node;
            else{
                prev_node->next = new_node;
                new_node->next = next_node;
            }
            break;
        }
        
        if (new_node->frequency <= next_node->frequency)
        {
            if (i == 0 || encoder->frequency_list->first_node->next == NULL){
            new_node->next = next_node;
            encoder->frequency_list->first_node = new_node;
            break;
            }
            prev_node->next = new_node;
            new_node->next = next_node;
            break;
        }

        if (encoder->frequency_list->first_node->next == NULL){
            encoder->frequency_list->first_node->next = new_node;
            break;
        }
        prev_node = next_node;
        next_node = next_node->next;
    }  

    encoder->frequency_list->size++;
}

Status create_frequency_list(Huffman_Encoder *encoder)
{
    log_message(LOG_DEBUG, "Creating frequency list.");
    encoder->frequency_list = (Frequency_List*) calloc(1, sizeof(Frequency_List));
    if (encoder->frequency_list == NULL){
            log_message(LOG_ERROR, "Failed to allocate memory for Frequency_List.");
            return ERROR_MEMORY_ALLOCATION;
        } 

    for (int i = 0; i < ASCII_SIZE; i++)
    {
        //Verifica se a letra aparece
        if (encoder->char_frequency[i] == 0)
            continue;
        
        //Cria o no
        Frequency_Node *new_node = (Frequency_Node*) calloc(1, sizeof(Frequency_Node));
        if (new_node == NULL){
            log_message(LOG_ERROR, "Failed to allocate memory for Frequency_Node.");
            return ERROR_MEMORY_ALLOCATION;
        }

        new_node->symbol = (unsigned char) i;
        new_node->frequency = encoder->char_frequency[i];
        insert_sorted_node(new_node, encoder);
    }   
    return STATUS_OK;
}

void test()
{
    logger_init("test_huff", 1);
    Huffman_Encoder *encoder;
    Status status;
    status = create_huffman_encoder(&encoder, "test.txt", "");
    if (status)
        //return;
    count_char_frequency(encoder);
    create_frequency_list(encoder);
    logger_close();
}
