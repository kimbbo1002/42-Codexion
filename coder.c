/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:25:58 by bokim             #+#    #+#             */
/*   Updated: 2026/04/14 19:22:40 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/codexion.h"

static void	*sim_one_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->lock);
	print_action(&coder, "has taken a dongle");
	controlled_sleep(&coder->hub->config->time_burnout, &coder->hub);
	pthread_mutex_unlock(&coder->left_dongle->lock);
}

static void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->left_dongle->lock);
	print_action(coder, "has taken a dongle");
	pthread_mutex_lock(&coder->right_dongle->lock);
	print_action(coder, "has taken a dongle");
	print_action(coder, "is compilling");
	controlled_sleep(coder->hub->config->time_compile, coder->hub);
	dongle_cooldown(coder);
	pthread_mutex_unlock(&coder->left_dongle->lock);
	pthread_mutex_unlock(&coder->right_dongle->lock);
	pthread_mutex_lock(&coder->compile_lock);
	coder->last_compile = get_time();
	coder->compile_count++;
	pthread_mutex_unlock(&coder->compile_lock);
}

static void	debug(t_coder *coder)
{
	print_action(coder, "is debugging");
	controlled_sleep(coder->hub->config->time_debug, coder->hub);
}

static void	refactor(t_coder *coder)
{
	print_action(coder, "is refactoring");
	controlled_sleep(coder->hub->config->time_burnout, coder->hub);
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
		compile(coder);
		debug(coder);
		refactor(coder);
	}
	return (NULL);
}
