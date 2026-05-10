/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pc <pc@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:25:58 by bokim             #+#    #+#             */
/*   Updated: 2026/05/10 18:31:25 by pc               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	heap_sift_down(t_heap *heap, t_heap_node last)
{
	int	i;
	int	child;

	i = 0;
	while (i * 2 + 1 < heap->size)
	{
		child = i * 2 + 1;
		if (child + 1 < heap->size && heap->nodes[child + 1].priority
			< heap->nodes[child].priority)
			child++;
		if (last.priority <= heap->nodes[child].priority)
			break ;
		heap->nodes[i] = heap->nodes[child];
		i = child;
	}
	heap->nodes[i] = last;
}

void	heap_pop(t_heap *heap)
{
	t_heap_node	last;

	pthread_mutex_lock(&heap->lock);
	if (!heap->size)
	{
		pthread_mutex_unlock(&heap->lock);
		return ;
	}
	last = heap->nodes[--heap->size];
	heap_sift_down(heap, last);
	pthread_cond_broadcast(&heap->update_cond);
	pthread_mutex_unlock(&heap->lock);
}

void	wait_for_turn(t_coder *coder)
{
	t_hub			*hub;
	unsigned long	priority;
	int				i;

	hub = coder->hub;
	pthread_mutex_lock(&hub->scheduler->lock);
	if (hub->config->scheduler == 1)
		priority = hub->count++;
	else
		priority = get_time() + hub->config->time_burnout;
	i = hub->scheduler->size++;
	while (i > 0 && priority < hub->scheduler->nodes[(i - 1) / 2].priority)
	{
		hub->scheduler->nodes[i] = hub->scheduler->nodes[(i - 1) / 2];
		i = (i - 1) / 2;
	}
	hub->scheduler->nodes[i].coder_id = coder->id;
	hub->scheduler->nodes[i].priority = priority;
	while (get_running_status(hub)
		&& hub->scheduler->nodes[0].coder_id != coder->id)
		pthread_cond_wait(&hub->scheduler->update_cond, &hub->scheduler->lock);
	pthread_mutex_unlock(&hub->scheduler->lock);
}
