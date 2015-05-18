# HTML-to-Drupal
Convenient class to create Drupal node from HTML
This should be a sample guide to parse HTML. 

This class was used to work with [GDocService](https://github.com/primerg/gdocs_service) to retrieve HTML and convert to Drupal node.

Tested in Drupal 6 ONLY. The node's structure will be different to Drupal 7 so adjust accordingly.

## EXAMPLE ##

```php
$doc = new HTML_to_Drupal($html);
$doc->getDrupalNode();

// returns a node object
```

## MODIFY ##

Modify getDrupalNode() to assign the right values to your node.