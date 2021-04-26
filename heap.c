#include "heap.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>

typedef struct heap_node_s{
    struct heap_node_s* root;
    struct heap_node_s* left;
    struct heap_node_s* right;
    bool has_data;
    void* data;
}heap_node_t;

typedef struct{
    int len;
    int item_size;
    void* buf;
    heap_node_t* node_mem;
    heap_node_t* entry;
    compare_t compare;
}heap_t;

int heap_create(heap_handle_t* p_heap,int len,int size,compare_t compare)
{
    // malloc all data at once
    *p_heap = malloc(sizeof(heap_t)+len*sizeof(heap_node_t)+len*size);
    if(*p_heap == NULL)
    {
        return -1;
    }
    memset(*p_heap,0,sizeof(heap_t)+len*sizeof(heap_node_t)+len*size);
    heap_t* heap = (heap_t*)(*p_heap);
    heap->buf = (void*)(*p_heap + sizeof(heap_t) + len*sizeof(heap_node_t));
    heap->len = len;
    heap->item_size = size;
    heap->node_mem = (heap_node_t*)(*p_heap + sizeof(heap_t));
    heap->entry = heap->node_mem;
    heap->compare = compare;
    // init heap
    for(int i=0;i<heap->len;i++)
    {
        heap_node_t* node = heap->node_mem + i;
        node->data = heap->buf + heap->item_size * i;
        node->has_data = false;
        node->root = NULL;
        node->left = NULL;
        node->right = NULL;
        if(i == 0)
        {
            continue;
        }
        int layer = 0;
        int cnt = 0;
        while(cnt < i+1)
        {
            cnt += 1<<layer;
            layer++;
        }
        int layer_index = (1<<(layer-1)) - (cnt - i);
        int root_index = ((1<<(layer-2))-1+layer_index/2);
        heap_node_t* root_node = heap->node_mem + root_index;
        node->root = root_node;
        if(layer_index%2==0)
        {
            root_node->left = node;
        }
        else
        {
            root_node->right = node;
        }
    }
    return 0;
}

bool heap_compare(heap_t* heap,heap_node_t* A,heap_node_t* B)
{
    // more than
    if(!A->has_data && B->has_data)
    {
        return true;
    }
    // less or equal to
    if(!B->has_data)
    {
        return false;
    }
    return heap->compare(A->data,B->data);
}

typedef enum{
    HEAP_ADJUST_NONE,
    HEAP_ADJUST_TWO,
    HEAP_ADJUST_THREE,
}heap_adjust_type_t;

void heap_adjust(heap_node_t* node,heap_t* heap)
{
    if(node->left == NULL)
    {
        // end point
        return;
    }
    heap_adjust_type_t adjust = HEAP_ADJUST_NONE;
    if(heap_compare(heap,node->left,node))
    {
        // left more than node
        if(node->right == NULL)
        {
            // exchange node and left
            adjust = HEAP_ADJUST_TWO;
        }
        else
        {
            if(heap_compare(heap,node->right,node))
            {
                // exchange three
                adjust = HEAP_ADJUST_THREE;
            }
            else
            {
                // exchange node and left
                adjust = HEAP_ADJUST_TWO;
            }
        }
    }
    if(adjust == HEAP_ADJUST_NONE)
    {
        return;
    }
    else if(adjust == HEAP_ADJUST_TWO)
    {
        if(node->root == NULL)
        {
            // entry node, change heap entry
            heap->entry = node->left;
        }
        else
        {
            if(node->root->right == node)
            {
                node->root->right = node->left;
            }
            else
            {
                node->root->left = node->left;
            }
        }
        heap_node_t* root_temp = node->root;
        heap_node_t* left_temp = node->left;
        heap_node_t* right_temp = node->right;
        node->root = left_temp;
        node->left = left_temp->left;
        if(node->left!=NULL)
        {
            node->left->root = node;
        }
        node->right = left_temp->right;
        if(node->right!=NULL)
        {
            node->right->root = node;
        }
        left_temp->root = root_temp;
        left_temp->left = node;
        // left_temp->left->root already set
        left_temp->right = right_temp;
        if(left_temp->right!=NULL)
        {
            left_temp->right->root = left_temp;
        }
        // adjust left node (which is node now)
        heap_adjust(node,heap);
    }
    else if(adjust == HEAP_ADJUST_THREE)
    {
        if(node->root == NULL)
        {
            heap->entry = node->left;
        }
        else
        {
            if(node->root->right == node)
            {
                node->root->right = node->left;
            }
            else
            {
                node->root->left = node->left;
            }
        }
        heap_node_t* root_temp = node->root;
        heap_node_t* left_temp = node->left;
        heap_node_t* right_temp = node->right;
        node->root = left_temp;
        node->left = right_temp->left;
        if(node->left!=NULL)
        {
            node->left->root = node;
        }
        node->right = right_temp->right;
        if(node->right!=NULL)
        {
            node->right->root = node;
        }
        right_temp->root = left_temp;
        right_temp->left = left_temp->left;
        if(right_temp->left!=NULL)
        {
            right_temp->left->root = right_temp;
        }
        right_temp->right = left_temp->right;
        if(right_temp->right!=NULL)
        {
            right_temp->right->root = right_temp;
        }
        left_temp->root = root_temp;
        left_temp->left = right_temp;
        left_temp->right = node;
        // adjust left node (which is right_temp now)
        heap_adjust(right_temp,heap);
        // adjust right node (which is node now) 
        heap_adjust(node,heap);
    }
}

int heap_add(heap_handle_t heap_handle,void* data)
{
    heap_t* heap = (heap_t*)heap_handle;
    bool able_to_enter = false;
    if(!heap->entry->has_data)
    {
        able_to_enter = true;
    }
    else
    {
        able_to_enter = heap->compare(heap->entry->data,data);
    }
    if(able_to_enter)
    {
        // copy over data
        memcpy(heap->entry->data,data,heap->item_size);
        heap->entry->has_data = true;
        // adjust heap
        heap_adjust(heap->entry,heap);
    }
    return 0;
}

void dump_hex(void *src, int len)
{
    for (int i = 0; i != len; i++)
    {
        printf("%02x ", *(unsigned char *)(src + i));
    }
    printf("\n");
}

void print_node(heap_node_t* node,size_t item_size,int layer)
{
    printf("layer: %d, self:%p, root:%p, left:%p, right:%p, has data:%s\n",
            layer,node,node->root,node->left,node->right,node->has_data?"Y":"N");
    if(node->has_data)
    {
        dump_hex(node->data,item_size);
    }
}

void print_heap2(heap_node_t* node,size_t item_size,int layer)
{
    print_node(node,item_size,layer++);
    if(node->left!=NULL)
    {
        print_heap2(node->left,item_size,layer);
    }
    if(node->right!=NULL)
    {
        print_heap2(node->right,item_size,layer);
    }
}

void print_heap(heap_handle_t handle)
{
    heap_t* heap = (heap_t*)handle;
    print_heap2(heap->entry,heap->item_size,1);
}

void count_data(heap_node_t* node,int* cnt)
{
    if(node == NULL)
    {
        return;
    }
    if(node->has_data)
    {
        *cnt = (*cnt) + 1;
    }
    count_data(node->left,cnt);
    count_data(node->right,cnt);
}

void get_data(heap_node_t* node,void* data,int* pos,size_t item_size)
{
    if(node == NULL)
    {
        return;
    }
    if(node->has_data)
    {
        memcpy(data+(*pos),node->data,item_size);
        *pos = (*pos) + item_size;
    }
    get_data(node->left,data,pos,item_size);
    get_data(node->right,data,pos,item_size);
}

void* heap_get_all(heap_handle_t handle,int* len)
{
    heap_t* heap = (heap_t*)handle;
    count_data(heap->entry,len);
    if(*len==0)
    {
        return NULL;
    }
    void* data = malloc((heap->item_size) * (*len));
    int pos = 0;
    get_data(heap->entry,data,&pos,heap->item_size);
    return data;
}

int heap_delete(heap_handle_t heap_handle)
{
    free(heap_handle);
}