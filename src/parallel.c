// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>

#include "os_graph.h"
#include "os_threadpool.h"
#include "log/log.h"
#include "utils.h"

#define NUM_THREADS		4

static int sum;
static os_graph_t *graph;
static os_threadpool_t *tp;

/* TODO: Define graph synchronization mechanisms. */
pthread_mutex_t graph_mutex;
pthread_cond_t graph_condition;

/* TODO: Define graph task argument. */
typedef struct {
    unsigned int node_id;
} graph_task_arg_t;

void task_sum(void *arg);

void process_node(unsigned int idx)
{
	graph_task_arg_t *task_arg = malloc(sizeof(graph_task_arg_t));
	DIE(task_arg == NULL, "malloc");

	task_arg->node_id = idx;
	os_task_t *task = create_task(task_sum, task_arg, free);
	enqueue_task(tp, task);
}

void task_sum(void *arg)
{
	//argumentul lui e id-ul vecinului
	// (incrementarea sumei si marcarea corecta a nodului)
	graph_task_arg_t *task_arg = (graph_task_arg_t *)arg;
	unsigned int idx = task_arg->node_id;

	pthread_mutex_lock(&graph_mutex);
	sum += graph->nodes[idx]->info;
	graph->visited[idx] = DONE;
	pthread_mutex_unlock(&graph_mutex);

	pthread_mutex_lock(&graph_mutex);
	// Create tasks and add them to the task queue for neighboring nodes
	for (unsigned int i = 0; i < graph->nodes[idx]->num_neighbours; ++i) {
		if (graph->visited[graph->nodes[idx]->neighbours[i]] == NOT_VISITED) {
			graph->visited[graph->nodes[idx]->neighbours[i]] = PROCESSING;
			log_debug("tid: %d, create task for %d\n", gettid(), graph->nodes[idx]->neighbours[i]);
			process_node(graph->nodes[idx]->neighbours[i]);
		}
	}
	pthread_mutex_unlock(&graph_mutex);
}

int main(int argc, char *argv[])
{

	FILE *input_file;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	input_file = fopen(argv[1], "r");
	DIE(input_file == NULL, "fopen");

	log_set_level(LOG_ERROR);
	graph = create_graph_from_file(input_file);

	/* TODO: Initialize graph synchronization mechanisms. */
	pthread_mutex_init(&graph_mutex, NULL);
	pthread_cond_init(&graph_condition, NULL);

	tp = create_threadpool(NUM_THREADS);
	process_node(0);

	wait_for_completion(tp);
	destroy_threadpool(tp);
	pthread_mutex_destroy(&graph_mutex);
	pthread_cond_destroy(&graph_condition);

	printf("%d", sum);

	return 0;
}
