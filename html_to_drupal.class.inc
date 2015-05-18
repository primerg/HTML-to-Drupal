<?php
class HTML_to_Drupal {
  private $rawHtml;
  private $result;
  private $drupalNode;

  public function __construct($html) {
    $this->rawHtml = $html;
    $this->result = $html;

    $this->drupalNode = new StdClass();
    $this->drupalNode->type = 'blog';
    $this->drupalNode->status = 0;
    $this->drupalNode->title = '';
    $this->drupalNode->body = '';
    $this->drupalNode->uid = 0;
  }

  public function getBody() {
    $dom = new DOMDocument();
    $dom->loadHTML($this->result);
    $body = $dom->getElementsByTagName('body');
    $body = $body->item(0);
    $this->result = $dom->savehtml($body);

    return $body;
  }

  public function cleanup() {
    $dom = new DOMDocument();
    $this->getBody();
    $dom->loadHTML($this->result);

    $xpath = new DOMXPath($dom);
    $nodes = $xpath->query('//@*');
    foreach ($nodes as $node) {
        $node->parentNode->removeAttribute($node->nodeName);
    }

    $newhtml = $dom->saveHTML();
    $this->result = str_replace('<a></a>', '', $newhtml);
  }

  /*
    This is the document's structure
    Title: --> Node title
    Tags: --> tags field
    Teaser: --> teaser field
    Meta Description: --> meta description field
    
    ... the rest is body ...
  */
  public function getDrupalNode() {
    $dom = new DOMDocument();
    $this->getBody();
    $this->cleanup();

    // Edit the parsed data here
    $dom->loadHTML($this->result);
    $xpath = new DOMXPath($dom);
    $nodes = $xpath->query('//p');

    foreach ($nodes as $node) {
      $key = $node->childNodes->item(0);
      $content = $node->childNodes->item(1);
      if (!$content) {
        continue;
      }

      switch ($key->nodeValue) {
        case 'Title':
          $this->drupalNode->title = str_replace(': ', '', $content->nodeValue);
          break;
        case 'Tags':
          $tags = explode(',', str_replace(': ', '', $content->nodeValue));
          $arr = array();
          foreach ($tags as $key => $value) {
              $arr[] = trim($value);
          }
          $this->drupalNode->taxonomy = $arr;
          break;
        case 'Teaser':
          $teaser = str_replace(': ', '', $content->nodeValue);
          $this->drupalNode->teaser = '';
          $this->drupalNode->field_blog_teaser[0]['value'] = $teaser;
          break;
        case 'Meta Description':
          $this->drupalNode->field_meta_description[0]['value'] = str_replace(': ', '', $content->nodeValue);
          break;
      }

      $node->parentNode->removeChild($node);

      // We need to have some breaker and for this scenario, we assume this is the breaker
      if (trim($content->nodeValue) == 'Meta Keywords') {
        break;
      }
    }

    $this->result = $dom->saveHTML();

    $res = preg_match('/<body[^>]*>(.*?)<\/body>/is', $this->result, $matches);
    
    $this->drupalNode->body = trim($matches[1]);
    $this->drupalNode->format = 2;

    return $this->drupalNode;
  }
}