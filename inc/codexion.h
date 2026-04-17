/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bokim <bokim@student.42lyon.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/06 13:54:43 by bokim             #+#    #+#             */
/*   Updated: 2026/04/17 16:20:59 by bokim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

// standard header files
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

// struct definitions
struct					s_hub;
typedef struct s_hub	t_hub;

typedef struct s_config
{
	int					num_coder;
	unsigned long		time_burnout;
	unsigned long		time_compile;
	unsigned long		time_debug;
	unsigned long		time_refactor;
	int					num_compiles;
	unsigned long		dongle_cooldown;
	int					scheduler;
}						t_config;

typedef struct s_dongle
{
	int					id;
	pthread_mutex_t		lock;
	unsigned long		last_used;
}						t_dongle;

typedef struct s_coder
{
	int					id;
	pthread_t			thread_id;
	int					compile_count;
	unsigned long		last_compile;
	pthread_mutex_t		compile_lock;

	t_dongle			*left_dongle;
	t_dongle			*right_dongle;

	t_hub				*hub;
}						t_coder;

typedef struct s_hub
{
	pthread_t			monitor_id;
	t_config			*config;
	t_coder				*coders;
	t_dongle			*dongles;

	unsigned long		start_time;
	int					running;
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		print_lock;
}						t_hub;

// parsing.c
int						parse_args(int argc, char **args, t_hub *hub);

// init.c
int						init_hub(t_hub *hub);

// hub.c
int						start_hub(t_hub *hub);
void					stop_hub(t_hub *hub);
void					clean_hub(t_hub *hub);
void					dongle_cooldown(t_coder *coder);

// coder.c
void					*coder_routine(void *coder_struct);

// dongle.c
void					yield_edf(t_coder *coder);

// monitor.c
void					*monitor_threads(void *hub_struct);

// utils.c
unsigned long			get_time(void);
int						get_running_status(t_hub *hub);
void					print_action(t_coder *coder, char *msg);
void					controlled_sleep(unsigned long sleep, t_hub *hub);
unsigned long			ft_atol(const char *str);

#endif