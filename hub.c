/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hub.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:46:33 by bokim             #+#    #+#             */
/*   Updated: 2026/04/14 19:14:10 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/codexion.h"

int	start_hub(t_hub *hub)
{
	int	i;

	hub->start_time = get_time();
	i = 0;
	while (i < hub->config->num_coder)
	{
		if (pthread_create(&hub->coders[i].thread_id, NULL, &coder_routine,
				&hub->coders[i]))
			return (0);
		i++;
	}
	if (pthread_create(&hub->monitor_id, NULL, &monitor_threads, hub))
		return (0);
	return (1);
}

int	stop_hub(t_hub *hub)
{
	int	i;

	pthread_join(hub->monitor_id, NULL);
	i = 0;
	while (i < hub->config->num_coder)
	{
		pthread_join(&hub->coders[i].thread_id, NULL);
		i++;
	}
}

int	clean_hub(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->config->num_coder)
	{
		pthread_mutex_destroy(&hub->dongles[i].lock);
		pthread_mutex_destroy(&hub->coders[i].compile_lock);
		i++;
	}
	pthread_mutex_destroy(&hub->stop_lock);
	pthread_mutex_destroy(&hub->print_lock);
	free(hub->coders);
	free(hub->dongles);
}
