#include "stdinc.h"
#include "ircd.h"
#include "client.h"
#include "modules.h"
#include "send.h"
#include "numeric.h"
#include "hash.h"

static int mo_oaccept(struct Client *client_p, struct Client *source_p, int parc, const char *parv[]);

struct Message oaccept_msgtab = {
	"OACCEPT", 0, 0, 0, MFLG_SLOW,
	{mg_unreg, mg_not_oper, mg_ignore, mg_ignore, mg_ignore, {mo_oaccept, 2}}
};  

mapi_clist_av1 oaccept_clist[] = { &oaccept_msgtab, NULL };

DECLARE_MODULE_AV1(oaccept, NULL, NULL, oaccept_clist, NULL, NULL, "$Id $");

static int
mo_oaccept(struct Client *client_p, struct Client *source_p, int parc, const char *parv[])
{
	struct Metadata *md;
	struct DictionaryIter iter;
	struct Client *target_p;
	char *text = rb_strdup("");

	if(!(target_p = find_client(parv[1])))
	{
		sendto_one(source_p, form_str(ERR_NOSUCHNICK), parv[1]);
		return 0;
	}

	rb_sprintf(text, "O%s", source_p->id);

	/* Don't allow someone to pointlessly fill up someone's metadata
	 * with identical OACCEPT entries. */
	DICTIONARY_FOREACH(md, &iter, target_p->user->metadata)
	{
		if(!strcmp(md->value, "OACCEPT") && !strcmp(md->name, text))
		{
			sendto_one_notice(source_p, ":You're already on %s's OACCEPT list", target_p->name);
			return 0;
		}
	}

	user_metadata_add(target_p, text, "OACCEPT", 1);

	sendto_wallops_flags(UMODE_WALLOP, &me,
			     "OACCEPT called for %s by %s!%s@%s",
			     target_p->name,
			     source_p->name, source_p->username, source_p->host);
		sendto_server(NULL, NULL, NOCAPS, NOCAPS, 
			      ":%s WALLOPS :OACCEPT called for %s by %s!%s@%s",
			      me.name, target_p->name, source_p->name, source_p->username,
			      source_p->host);
	return 0;
}