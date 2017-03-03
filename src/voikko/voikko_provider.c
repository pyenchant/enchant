/* enchant
 * Copyright (C) 2003,2004 Dom Lachowicz
 *               2006-2007 Harri Pitkänen <hatapitk@iki.fi>
 *               2006 Anssi Hannula <anssi.hannula@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02110-1301, USA.
 *
 * In addition, as a special exception, Dom Lachowicz
 * gives permission to link the code of this program with
 * non-LGPL Spelling Provider libraries (eg: a MSFT Office
 * spell checker backend) and distribute linked combinations including
 * the two.  You must obey the GNU Lesser General Public License in all
 * respects for all of the code used other than said providers.  If you modify
 * this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to
 * do so, delete this exception statement from your version.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <libvoikko/voikko.h>
#include "unused-parameter.h"

#include "enchant.h"
#include "enchant-provider.h"

/**
 * Voikko is a Finnish spell checker. More information is available from:
 *
 * http://voikko.sourceforge.net/
 */

static int
voikko_dict_check (EnchantDict * me, const char *const word, size_t len _GL_UNUSED_PARAMETER)
{
	int result;
	int voikko_handle;

	voikko_handle = (long) me->user_data;
	result = voikko_spell_cstr(voikko_handle, word);
	if (result == VOIKKO_SPELL_FAILED)
		return 1;
	else if (result == VOIKKO_SPELL_OK)
		return 0;
	else
		return -1;
}

static char **
voikko_dict_suggest (EnchantDict * me, const char *const word,
		     size_t len _GL_UNUSED_PARAMETER, size_t * out_n_suggs)
{
	char **sugg_arr;
	int voikko_handle;

	voikko_handle = (long) me->user_data;
	sugg_arr = voikko_suggest_cstr(voikko_handle, word);
	if (sugg_arr == NULL)
		return NULL;
	for (*out_n_suggs = 0; sugg_arr[*out_n_suggs] != NULL; (*out_n_suggs)++);
	return sugg_arr;
}

static EnchantDict *
voikko_provider_request_dict (EnchantProvider * me, const char *const tag)
{
	EnchantDict *dict;
	const char * voikko_error;
	int voikko_handle;

	/* Only Finnish is supported at the moment */
	if (strncmp(tag, "fi_FI", 6) != 0 && strncmp(tag, "fi", 3) != 0)
		return NULL;
	
	voikko_error = voikko_init(&voikko_handle, "fi_FI", 0);
	if (voikko_error) {
		enchant_provider_set_error(me, voikko_error);
		return NULL;
	}

	dict = g_new0 (EnchantDict, 1);
	dict->user_data = (void *)(long) voikko_handle;
	dict->check = voikko_dict_check;
	dict->suggest = voikko_dict_suggest;

	return dict;
}

static void
voikko_provider_dispose_dict (EnchantProvider * me _GL_UNUSED_PARAMETER, EnchantDict * dict)
{
	voikko_terminate((long) dict->user_data);
	g_free (dict);
}

static int
voikko_provider_dictionary_exists (struct str_enchant_provider * me _GL_UNUSED_PARAMETER,
                                   const char *const tag)
{
	int voikko_handle;

	/* Only Finnish is supported */
	if (strncmp(tag, "fi", 3) != 0)
		return 0;

	/* Check that a dictionary is actually available */
	if (voikko_init(&voikko_handle, "fi_FI", 0) == NULL) {
		voikko_terminate(voikko_handle);
		return 1;
	}
	else return 0;
}


static char **
voikko_provider_list_dicts (EnchantProvider * me _GL_UNUSED_PARAMETER, 
			    size_t * out_n_dicts)
{
	char ** out_list = NULL;
	int voikko_handle;
	*out_n_dicts = 0;

	if (voikko_init(&voikko_handle, "fi_FI", 0) == NULL) {
		voikko_terminate(voikko_handle);
		*out_n_dicts = 1;
		out_list = g_new0 (char *, *out_n_dicts + 1);
		out_list[0] = g_strdup("fi");
	}

	return out_list;
}

static void
voikko_provider_free_string_list (EnchantProvider * me _GL_UNUSED_PARAMETER,
				  char **str_list)
{
	g_strfreev (str_list);
}

static void
voikko_provider_dispose (EnchantProvider * me)
{
	g_free (me);
}

static const char *
voikko_provider_identify (EnchantProvider * me _GL_UNUSED_PARAMETER)
{
	return "voikko";
}

static const char *
voikko_provider_describe (EnchantProvider * me _GL_UNUSED_PARAMETER)
{
	return "Voikko Provider";
}

ENCHANT_MODULE_EXPORT (EnchantProvider *) 
	     init_enchant_provider (void);

EnchantProvider *
init_enchant_provider (void)
{
	EnchantProvider *provider;

	provider = g_new0 (EnchantProvider, 1);
	provider->dispose = voikko_provider_dispose;
	provider->request_dict = voikko_provider_request_dict;
	provider->dispose_dict = voikko_provider_dispose_dict;
	provider->dictionary_exists = voikko_provider_dictionary_exists;
	provider->identify = voikko_provider_identify;
	provider->describe = voikko_provider_describe;
	provider->list_dicts = voikko_provider_list_dicts;
	provider->free_string_list = voikko_provider_free_string_list;

	return provider;
}
