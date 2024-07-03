/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jstrozyk <jstrozyk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/01 16:14:51 by evan-ite          #+#    #+#             */
/*   Updated: 2024/07/03 11:04:35 by jstrozyk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <cstring>

std::string findKey(std::string file_str, std::string keyword, char separator);
std::string splitReturnFirst(const std::string& str, const std::string& delimiter);

#endif
