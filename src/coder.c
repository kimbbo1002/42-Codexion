/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pc <pc@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 15:25:58 by bokim             #+#    #+#             */
/*   Updated: 2026/05/10 17:23:23 by pc               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		wait_for_turn(coder);
		if (compile(coder))
			break ;
		heap_pop(coder->hub->scheduler);
		if (debug(coder) || refactor(coder))
			break ;
	}
	return (NULL);
}
