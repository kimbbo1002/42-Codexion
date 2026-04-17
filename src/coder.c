/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:25:58 by bokim             #+#    #+#             */
/*   Updated: 2026/04/17 15:55:38 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	*sim_one_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->lock);
	print_action(coder, "has taken a dongle");
	controlled_sleep(coder->hub->config->time_burnout, coder->hub);
	pthread_mutex_unlock(&coder->left_dongle->lock);
	return (NULL);
}

static int	compile(t_coder *coder)
{
	if (!get_running_status(coder->hub))
		return (1);
	pthread_mutex_lock(&coder->left_dongle->lock);
	print_action(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->right_dongle->lock);
	print_action(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->compile_lock);
	coder->last_compile = get_time();
	pthread_mutex_unlock(&coder->compile_lock);
	print_action(coder, "is compiling");
	controlled_sleep(coder->hub->config->time_compile, coder->hub);
	dongle_cooldown(coder);
	pthread_mutex_unlock(&coder->left_dongle->lock);
	pthread_mutex_unlock(&coder->right_dongle->lock);
	pthread_mutex_lock(&coder->compile_lock);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->compile_lock);
	return (0);
}

static int	debug(t_coder *coder)
{
	if (!get_running_status(coder->hub))
		return (1);
	print_action(coder, "is debugging");
	controlled_sleep(coder->hub->config->time_debug, coder->hub);
	return (0);
}

static int	refactor(t_coder *coder)
{
	if (!get_running_status(coder->hub))
		return (1);
	print_action(coder, "is refactoring");
	controlled_sleep(coder->hub->config->time_refactor, coder->hub);
	return (0);
}

void	*coder_routine(void *coder_struct)
{
	t_coder	*coder;

	coder = (t_coder *)coder_struct;
	if (coder->hub->config->num_coder == 1)
		return (sim_one_coder(coder));
	if (coder->id % 2 == 0)
		usleep(1000);
	while (get_running_status(coder->hub))
	{
		if (!coder->hub->config->scheduler)
			yield_edf(coder);
		if (compile(coder) || debug(coder) || refactor(coder))
			break ;
	}
	return (NULL);
}
