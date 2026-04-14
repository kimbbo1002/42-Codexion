/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 18:45:33 by bokim             #+#    #+#             */
/*   Updated: 2026/04/14 19:14:20 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/codexion.h"

static int	check_burnouts(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->config->num_coder)
	{
		pthread_mutex_lock(&hub->coders[i].compile_lock);
		if (get_time()
			- hub->coders[i].last_compile >= hub->config->time_burnout)
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->running = false;
			pthread_mutex_unlock(&hub->stop_lock);
			print_action(&hub->coders[i], "burned out");
			pthread_mutex_unlock(&hub->coders[i].compile_lock);
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
	return (finished == hub->config->num_compiles);
}

void	*monitor_threads(void *hub_struct)
{
	t_hub	*hub;

	hub = (t_hub *)hub_struct;
	while (get_running_status(hub))
	{
		if (check_burnouts(hub))
			break ;
		if (check_burnouts(hub))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->running = false;
			break ;
		}
		usleep(1000);
	}
	return (NULL);
}
