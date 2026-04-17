/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/16 08:43:04 by bokim             #+#    #+#             */
/*   Updated: 2026/04/17 16:21:27 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static unsigned long	get_deadline(t_coder *coder)
{
	unsigned long	deadline;

	pthread_mutex_lock(&coder->compile_lock);
	deadline = coder->last_compile + coder->hub->config->time_burnout;
	pthread_mutex_unlock(&coder->compile_lock);
	return (deadline);
}

void	yield_edf(t_coder *coder)
{
	t_coder			*left;
	t_coder			*right;
	unsigned long	my_deadline;

	left = &coder->hub->coders[(coder->id - 2 + coder->hub->config->num_coder)
		% coder->hub->config->num_coder];
	right = &coder->hub->coders[coder->id % coder->hub->config->num_coder];
	my_deadline = get_deadline(coder);
	if (get_deadline(left) < my_deadline || get_deadline(right) < my_deadline)
		controlled_sleep(1000, coder->hub);
}
