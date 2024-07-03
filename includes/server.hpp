/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ubazzane <ubazzane@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 16:14:51 by evan-ite          #+#    #+#             */
/*   Updated: 2024/07/02 15:19:49 by ubazzane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>
//# include "Response.hpp"

# define HTTPVERSION "HTTP/1.1"

std::string find_key(std::string file_str, std::string keyword, char separator);

#endif
