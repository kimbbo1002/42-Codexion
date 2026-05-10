/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pc <pc@student.42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 14:48:41 by bokim             #+#    #+#             */
/*   Updated: 2026/05/10 17:19:17 by pc               ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_hub		hub;
	t_config	config;
	t_heap		scheduler;

	hub.config = &config;
	hub.scheduler = &scheduler;
	if (!parse_args(argc, argv, &hub))
		return (0);
	if (!init_hub(&hub))
		return (0);
	if (!start_hub(&hub))
		return (0);
	stop_hub(&hub);
	clean_hub(&hub);
}
