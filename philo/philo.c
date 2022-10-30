/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aainhaja <aainhaja@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/26 18:54:54 by aainhaja          #+#    #+#             */
/*   Updated: 2022/10/16 23:24:50 by aainhaja         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time_now(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	check_death(t_philo *arg)
{	
	long	i;

	i = get_time_now();
	pthread_mutex_lock(&arg->set->eat);
	if (i - arg->time_start > arg->time_to_die)
	{
		pthread_mutex_lock(&arg->set->dead);
		pthread_mutex_lock(&arg->set->write);
		printf("%ld %d%s", (get_time_now() - arg->time_beg),
			arg->i + 1, " Died\n");
		arg->set->death = 0;
	}
	else
	{
		if (arg->nb_of_eat != -1)
		{
			if (arg->k[0] == 0)
			{
				arg->set->eated = 0;
				exit(1);
			}
		}
		pthread_mutex_unlock(&arg->set->eat);
		pthread_mutex_unlock(&arg->set->time);
	}
}

void	eating(t_philo *philo)
{
	long	start;

	pthread_mutex_lock(&philo->set->eat);
	if (philo->nb_of_eat > 0)
	{
		philo->nb_of_eat--;
		philo->k[0]--;
	}
	start = get_time_now();
	philo->time_start = get_time_now();
	pthread_mutex_unlock(&philo->set->eat);
	ft_print("", philo, " is eating\n");
	while (get_time_now() - start < philo->time_to_eat)
	{
		usleep(100);
	}
}

void	sleeping(t_philo *philo)
{
	long	i;

	ft_print("", philo, " is sleeping\n");
	i = get_time_now();
	while (get_time_now() - i < philo->time_to_sleep)
	{
		usleep(100);
	}
	ft_print("", philo, " is thinking\n");
	if (philo->nb_of_eat == 0)
		philo->nb_of_eat = -2;
}

void	routine(void *arg)
{
	t_philo	*philo;

	philo = arg;
	while (1)
	{
		pthread_mutex_lock(&philo->set->dead);
		if (philo->set->death == 0)
			break ;
		pthread_mutex_unlock(&philo->set->dead);
		if (philo->i % 2)
			usleep(1500);
		pthread_mutex_lock(&philo->set->mutex[philo->i]);
		ft_print("", philo, " has taken a fork\n");
		pthread_mutex_lock(&philo->set->mutex[(philo->i + 1) % philo->nb]);
		ft_print("", philo, " has taken a fork\n");
		eating(philo);
		pthread_mutex_unlock(&philo->set->mutex[philo->i]);
		pthread_mutex_unlock(&philo->set->mutex[(philo->i + 1) % philo->nb]);
		sleeping(philo);
	}
}

void philosophers(t_philo arg)
{
	pthread_t	*t;
    int			i;
	int			*all;
	t_philo	*philo;
	t_philo	*head;
	t_inside	*tools;

	all = malloc(sizeof(int));
	if (arg.nb_of_eat != -1)
	{
		all[0] = (arg.nb * arg.nb_of_eat);
	}
	tools = malloc(sizeof(t_inside));
	tools->mutex = malloc(sizeof(pthread_mutex_t) * arg.nb);
	tools->death = 1;
	tools->eated = 1;
	tools->rc = malloc(sizeof(pthread_mutex_t) * arg.nb);
	t = malloc(sizeof(pthread_t) * arg.nb);
	pthread_mutex_init(&tools->dead, NULL);
	pthread_mutex_init(&tools->eat, NULL);
	pthread_mutex_init(&tools->write, NULL);
	pthread_mutex_init(&tools->time, NULL);
	philo = NULL;
	i = 0;
	while (i < arg.nb)
	{
		pthread_mutex_init(&tools->mutex[i], NULL);
		pthread_mutex_init(&tools->rc[i], NULL);
		i++;
	}
	i = arg.nb - 1;
	while (0 <= i)
	{
		head = (t_philo *) malloc(sizeof(t_philo));
		head->time_beg = arg.time_beg;
		head->nb = arg.nb;
		head->k = all;
		head->time_beg = arg.time_beg;
		head->time_start = arg.time_start;
		head->time_to_die = arg.time_to_die;
		head->time_to_eat = arg.time_to_eat;
		head->time_to_sleep = arg.time_to_sleep;
		head->nb_of_eat = arg.nb_of_eat;
		head->set = tools;
		head->next = philo;
		philo = head;
		i--;
	}
	i = 0;
	head = philo;
	while (i < arg.nb)
	{
		head->i = i;
		pthread_create(&t[i],NULL, (void *) routine, head);
		head = head->next;
		i += 2;
	}
	usleep(100);
	i = 1;
	while (i < arg.nb)
	{
		head->i = i;
		pthread_create(&t[i],NULL, (void *) routine, head);
		head = head->next;
		i += 2;
	}
	head = philo;
	i = 1;
	while (head->set->eated && head->set->death)
	{
		check_death(head);
		if (i == head->nb)
		{
			i = 1;
			head = philo;
			continue ;
		}
		head = head->next;
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_philo	arg;

	if (argc == 5 || argc == 6)
	{
		arg.time_beg = get_time_now();
		arg.nb = atoi(argv[1]);
		arg.time_start = get_time_now();
		arg.time_to_die = atoi(argv[2]);
		arg.time_to_eat = atoi(argv[3]);
		arg.time_to_sleep = atoi(argv[4]);
		if (argc == 5)
			arg.nb_of_eat = -1;
		else
			arg.nb_of_eat = atoi(argv[5]);
		philosophers(arg);
	}
}