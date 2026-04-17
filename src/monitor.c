/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 18:45:33 by bokim             #+#    #+#             */
/*   Updated: 2026/04/17 16:20:07 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnouts(t_hub *hub)
{
	int				i;
	unsigned long	time;

	i = 0;
	while (i < hub->config->num_coder)
	{
		pthread_mutex_lock(&hub->coders[i].compile_lock);
		if (get_time()
			- hub->coders[i].last_compile >= hub->config->time_burnout)
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->running = 0;
			pthread_mutex_unlock(&hub->stop_lock);
			pthread_mutex_unlock(&hub->coders[i].compile_lock);
			pthread_mutex_lock(&hub->print_lock);
			time = get_time() - hub->start_time;
			printf("%lu %d burned out\n", time, hub->coders[i].id);
			pthread_mutex_unlock(&hub->print_lock);
			return (1);
		}
		pthread_mutex_unlock(&hub->coders[i].compile_lock);
		i++;
	}
	return (0);
}

static int	check_completion(t_hub *hub)
{
	int	i;
	int	finished;

	i = 0;
	finished = 0;
	while (i < hub->config->num_coder)
	{
		pthread_mutex_lock(&hub->coders[i].compile_lock);
		if (hub->coders[i].compile_count >= hub->config->num_compiles)
			finished++;
		pthread_mutex_unlock(&hub->coders[i].compile_lock);
		i++;
	}
	return (finished == hub->config->num_coder);
}

void	*monitor_threads(void *hub_struct)
{
	t_hub			*hub;
	unsigned long	time;

	hub = (t_hub *)hub_struct;
	while (get_running_status(hub))
	{
		if (check_burnouts(hub))
			break ;
		if (check_completion(hub))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->running = 0;
			pthread_mutex_unlock(&hub->stop_lock);
			pthread_mutex_lock(&hub->print_lock);
			time = get_time() - hub->start_time;
			printf("%lu all coders have completed %i compiles\n", time,
				hub->config->num_compiles);
			pthread_mutex_unlock(&hub->print_lock);
			break ;
		}
	}
	return (NULL);
}
