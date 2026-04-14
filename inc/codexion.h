/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 13:54:43 by bokim             #+#    #+#             */
/*   Updated: 2026/04/14 19:14:37 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_config
{
	int				num_coder;
	int				time_burnout;
	int				time_compile;
	int				time_debug;
	int				time_refactor;
	int				num_compiles;
	int				dongle_cooldown;
	int				scheduler;
}					t_config;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	lock;
	unsigned long	last_used;
}					t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread_id;
	int				compile_count;
	unsigned long	last_compile;
	pthread_mutex_t	compile_lock;

	t_dongle		*left_dongle;
	t_dongle		*right_dongle;

	t_hub			*hub;
}					t_coder;

typedef struct s_hub
{
	pthread_t		monitor_id;
	t_config		*config;
	t_coder			*coders;
	t_dongle		*dongles;

	unsigned long	start_time;
	bool			running;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	print_lock;
}					t_hub;

int		parse_args(int argc, char **args, t_config *config);
int		get_running_status(t_hub *hub);
void	print_action(t_coder *coder, char *msg);
void	controlled_sleep(unsigned long sleep, t_hub *hub);
void	*coder_routine(void *coder_struct);
void	*monitor_threads(void *hub_struct);

#endif