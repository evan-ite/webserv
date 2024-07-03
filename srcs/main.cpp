/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 16:12:24 by evan-ite          #+#    #+#             */
/*   Updated: 2024/07/03 10:39:00 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Logger.hpp"

int main(void)
{
	log(logINFO) << "INFO message";
	log(logDEBUG) << "DEBUG message";
	log(logERROR) << "ERROR message";
	log(logWARNING) << "WARNING message";
}
